/* Okur-yazar problemi (okur oncelikli): ilk okur veritabanini (db) kilitler, son
 * okur birakir; yazar db'yi tek basina alir. 4 okur ve 2 yazar sinirli tur calisir;
 * kural ihlali (okurla yazar ya da iki yazar ayni anda) sayilir. */
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>

#define OKUR 4
#define YAZAR 2
#define TUR 2000                 /* while (true) yerine */

typedef sem_t semaphore;
static void down(semaphore *s) { sem_wait(s); }
static void up(semaphore *s)   { sem_post(s); }

semaphore mutex;      /* rc'yi korur        = 1 */
semaphore db;         /* veritabanini korur = 1 */
int rc = 0;           /* okuyan sayisi      */

static long veri = 0;                     /* "veritabani" */
static atomic_int okuyan = 0, yazan = 0, ihlal = 0;
static atomic_int en_cok_okur = 0;
static atomic_long okuma = 0, yazma = 0;

static void think_up_data(void) { }
static void use_data_read(void) { }

static void write_data_base(void)
{
    if (atomic_fetch_add(&yazan, 1) != 0 || atomic_load(&okuyan) != 0)
        atomic_fetch_add(&ihlal, 1);
    veri++;
    atomic_fetch_add(&yazma, 1);
    atomic_fetch_sub(&yazan, 1);
}

static void read_data_base(void)
{
    int o = atomic_fetch_add(&okuyan, 1) + 1;
    int m = atomic_load(&en_cok_okur);
    while (o > m && !atomic_compare_exchange_weak(&en_cok_okur, &m, o))
        ;
    if (atomic_load(&yazan) != 0)
        atomic_fetch_add(&ihlal, 1);
    long v = veri;                        /* oku */
    (void)v;
    atomic_fetch_add(&okuma, 1);
    atomic_fetch_sub(&okuyan, 1);
}

void *writer(void *arg)
{
    (void)arg;
    for (int k = 0; k < TUR; k++) {
        think_up_data();
        down(&db);  write_data_base();  up(&db);
    }
    return NULL;
}

void *reader(void *arg)
{
    (void)arg;
    for (int k = 0; k < TUR; k++) {
        down(&mutex);
        if (++rc == 1) down(&db);  /* ilk okur  */
        up(&mutex);
        read_data_base();
        down(&mutex);
        if (--rc == 0) up(&db);    /* son okur  */
        up(&mutex);
        use_data_read();
    }
    return NULL;
}

int main(void)
{
    pthread_t o[OKUR], y[YAZAR];
    sem_init(&mutex, 0, 1);
    sem_init(&db, 0, 1);
    for (int i = 0; i < OKUR; i++)
        pthread_create(&o[i], NULL, reader, NULL);
    for (int i = 0; i < YAZAR; i++)
        pthread_create(&y[i], NULL, writer, NULL);
    for (int i = 0; i < OKUR; i++)
        pthread_join(o[i], NULL);
    for (int i = 0; i < YAZAR; i++)
        pthread_join(y[i], NULL);

    printf("okuma = %ld, yazma = %ld, veri = %ld\n",
           atomic_load(&okuma), atomic_load(&yazma), veri);
    printf("kural ihlali = %d, rc = %d\n", atomic_load(&ihlal), rc);
    printf("ayni anda en cok okur = %d (degisir, 1..%d)\n",
           atomic_load(&en_cok_okur), OKUR);
    sem_destroy(&mutex);
    sem_destroy(&db);
    return 0;
}

/* Beklenen cikti (son satir zamanlamaya bagli):
 * okuma = 8000, yazma = 4000, veri = 4000
 * kural ihlali = 0, rc = 0
 * ayni anda en cok okur = N (degisir, 1..4)
 */
