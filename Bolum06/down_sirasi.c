/* down sirasi degisirse: uretici once mutex'i, sonra empty'yi alir. Tampon doluyken
 * uretici mutex'i tutarak empty'de bloke olur; tuketici full'u alir ama mutex'te
 * bloke olur: kilitlenme. Sonsuza dek beklemek yerine down() 1 saniyelik zaman
 * asimiyla doner ve program durumu raporlayip biter. */
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <time.h>
#include <errno.h>

#define N 2
#define ADET 5

typedef sem_t semaphore;

static atomic_int kilitlenen = 0;

/* down: 1 s icinde alinamazsa 0 dondurur (kilitlenme belirtisi) */
static int down(semaphore *s)
{
    struct timespec son;
    clock_gettime(CLOCK_REALTIME, &son);
    son.tv_sec += 1;
    while (sem_timedwait(s, &son) != 0) {
        if (errno == ETIMEDOUT) {
            atomic_fetch_add(&kilitlenen, 1);
            return 0;
        }
    }
    return 1;
}
static void up(semaphore *s) { sem_post(s); }

semaphore mutex, empty, full;     /* 1, N, 0 */

static int tampon[N], giris = 0, cikis = 0, sonraki = 0;
static const char *uretici_nerede = "bitti", *tuketici_nerede = "bitti";
static atomic_int tuketici_basla = 0;

static int produce_item(void) { return ++sonraki; }
static void insert_item(int item) { tampon[giris] = item; giris = (giris + 1) % N; }
static int remove_item(void) { int x = tampon[cikis]; cikis = (cikis + 1) % N; return x; }
static void consume_item(int item) { (void)item; }

void *producer(void *arg)
{
    int item;
    (void)arg;
    for (int k = 0; k < ADET; k++) {
        item = produce_item();
        if (!down(&mutex)) { uretici_nerede = "mutex"; return NULL; }  /* <- once */
        if (k == N)                    /* tampon dolu: tuketici simdi baslasin */
            atomic_store(&tuketici_basla, 1);
        if (!down(&empty)) {                                            /* <- sonra */
            uretici_nerede = "empty (mutex elinde)";
            return NULL;    /* gercekte sonsuza dek bekler; mutex hic birakilmaz */
        }
        insert_item(item);
        up(&mutex);
        up(&full);
    }
    return NULL;
}

void *consumer(void *arg)
{
    int item;
    (void)arg;
    while (!atomic_load(&tuketici_basla)) {   /* tampon dolana dek gecik */
        struct timespec ts = { 0, 1000000 };
        nanosleep(&ts, NULL);
    }
    for (int k = 0; k < ADET; k++) {
        if (!down(&full))  { tuketici_nerede = "full";  return NULL; }
        if (!down(&mutex)) { tuketici_nerede = "mutex (full alindi)"; return NULL; }
        item = remove_item();
        up(&mutex);
        up(&empty);
        consume_item(item);
    }
    return NULL;
}

int main(void)
{
    pthread_t u, t;
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, N);
    sem_init(&full, 0, 0);
    pthread_create(&u, NULL, producer, NULL);
    pthread_create(&t, NULL, consumer, NULL);
    pthread_join(u, NULL);
    pthread_join(t, NULL);

    printf("uretici  bekledigi yer: %s\n", uretici_nerede);
    printf("tuketici bekledigi yer: %s\n", tuketici_nerede);
    printf("zaman asimina ugrayan down: %d -> %s\n", atomic_load(&kilitlenen),
           atomic_load(&kilitlenen) == 2 ? "kilitlenme" : "kilitlenme yok");
    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
    return 0;
}

/* Beklenen cikti (her calistirmada ayni; yaklasik 1 saniye surer):
 * uretici  bekledigi yer: empty (mutex elinde)
 * tuketici bekledigi yer: mutex (full alindi)
 * zaman asimina ugrayan down: 2 -> kilitlenme
 */
