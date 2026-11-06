/* Sinirli tampon (uretici-tuketici) uc semaforla: mutex tampona erisimi,
 * empty bos yuvalari, full dolu yuvalari sayar. Uretici 1..1000 uretir,
 * tuketici hepsini sirayla tuketir. */
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

#define N 100
#define ADET 1000                 /* while (true) yerine ADET oge */

typedef sem_t semaphore;
static void down(semaphore *s) { sem_wait(s); }
static void up(semaphore *s)   { sem_post(s); }

semaphore mutex;   /* tampona erisim  = 1 */
semaphore empty;   /* bos yuva sayisi = N */
semaphore full;    /* dolu yuva sayisi = 0 */

static int tampon[N];
static int giris = 0, cikis = 0;
static int sonraki = 0;
static long toplam = 0;
static int sira_bozuk = 0, beklenen = 1;

static int produce_item(void) { return ++sonraki; }
static void insert_item(int item) { tampon[giris] = item; giris = (giris + 1) % N; }
static int remove_item(void) { int x = tampon[cikis]; cikis = (cikis + 1) % N; return x; }
static void consume_item(int item)
{
    if (item != beklenen++)
        sira_bozuk = 1;
    toplam += item;
}

void *producer(void *arg)
{
    int item;
    (void)arg;
    for (int k = 0; k < ADET; k++) {
        item = produce_item();
        down(&empty);   /* bos yuva bekle   */
        down(&mutex);   /* KB'ye gir        */
        insert_item(item);
        up(&mutex);     /* KB'den cik       */
        up(&full);      /* dolu sayisi + 1  */
    }
    return NULL;
}

void *consumer(void *arg)
{
    int item;
    (void)arg;
    for (int k = 0; k < ADET; k++) {
        down(&full);    /* dolu yuva bekle  */
        down(&mutex);
        item = remove_item();
        up(&mutex);
        up(&empty);     /* bos sayisi + 1   */
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

    int e, f;
    sem_getvalue(&empty, &e);
    sem_getvalue(&full, &f);
    printf("tuketilen toplam = %ld (1 + ... + %d)\n", toplam, ADET);
    printf("sira %s\n", sira_bozuk ? "BOZUK" : "korundu");
    printf("empty = %d, full = %d\n", e, f);
    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
    return 0;
}

/* Beklenen cikti (her calistirmada ayni):
 * tuketilen toplam = 500500 (1 + ... + 1000)
 * sira korundu
 * empty = 100, full = 0
 */
