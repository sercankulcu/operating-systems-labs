/* Sinirli tampon adim adim: iki yuvali tampon (empty = 2), semaforlar wait/signal
 * adlariyla. Once uretici tek basina calisir: iki oge koyar ve ucuncude empty'de
 * bloke olur (mutex = 1, empty = 0, full = 2). Sonra tuketici baslar ve 5 ogenin
 * hepsi sirayla gecer. */
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

/* wait ve signal <sys/wait.h> / <signal.h> adlariyla karismasin diye
 * basliklardan sonra yeniden adlandirilir. */
#define wait   semafor_wait
#define signal semafor_signal

#define ADET 5

typedef sem_t semaphore;
static void wait(semaphore *s)   { sem_wait(s); }
static void signal(semaphore *s) { sem_post(s); }

semaphore mutex, empty, full;    /* mutex = 1, empty = 2, full = 0 */

static int tampon[2];
static int giris = 0, cikis = 0, sonraki = 0;
static int tuketilen[ADET], tn = 0;

static int uret(void) { return ++sonraki; }
static void tampona_koy(int item) { tampon[giris] = item; giris = (giris + 1) % 2; }
static int tampondan_al(void) { int x = tampon[cikis]; cikis = (cikis + 1) % 2; return x; }
static void tuket(int item) { tuketilen[tn++] = item; }

void *uretici(void *arg)
{
    int item;
    (void)arg;
    for (int k = 0; k < ADET; k++) {     /* while (true) yerine ADET tur */
        item = uret();
        wait(&empty);
        wait(&mutex);
        tampona_koy(item);
        signal(&mutex);
        signal(&full);
    }
    return NULL;
}

void *tuketici(void *arg)
{
    int item;
    (void)arg;
    for (int k = 0; k < ADET; k++) {
        wait(&full);
        wait(&mutex);
        item = tampondan_al();
        signal(&mutex);
        signal(&empty);
        tuket(item);
    }
    return NULL;
}

static void durum(const char *baslik)
{
    int m, e, f;
    sem_getvalue(&mutex, &m);
    sem_getvalue(&empty, &e);
    sem_getvalue(&full, &f);
    printf("%-26s mutex = %d, empty = %d, full = %d\n", baslik, m, e, f);
}

int main(void)
{
    pthread_t u, t;
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, 2);
    sem_init(&full, 0, 0);
    durum("baslangic:");

    pthread_create(&u, NULL, uretici, NULL);
    for (;;) {                           /* uretici iki oge koyup bloke olana dek */
        int f;
        sem_getvalue(&full, &f);
        if (f == 2)
            break;
        struct timespec ts = { 0, 1000000 };
        nanosleep(&ts, NULL);
    }
    struct timespec ts = { 0, 20000000 }; /* 20 ms: uretici empty'de bekliyor */
    nanosleep(&ts, NULL);
    durum("uretici empty'de bloke:");

    pthread_create(&t, NULL, tuketici, NULL);
    pthread_join(u, NULL);
    pthread_join(t, NULL);
    durum("son:");
    printf("tuketilen:");
    for (int i = 0; i < tn; i++)
        printf(" %d", tuketilen[i]);
    printf("\n");
    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
    return 0;
}

/* Beklenen cikti (her calistirmada ayni):
 * baslangic:                 mutex = 1, empty = 2, full = 0
 * uretici empty'de bloke:    mutex = 1, empty = 0, full = 2
 * son:                       mutex = 1, empty = 2, full = 0
 * tuketilen: 1 2 3 4 5
 */
