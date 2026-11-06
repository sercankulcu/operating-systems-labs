/* Okur-yazar, Downey'in acliksiz cozumu: herkes once ayni turnikeden gecer.
 * Yazar turnikeyi tutarken yeni okur giremez, bu yuzden surekli okur akisi
 * yazari aclikta birakamaz. Ilk okur odaBos'u alir, son okur birakir (rc ile). */
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>

/* wait/signal <sys/wait.h> ve <signal.h> ile karismasin diye yeniden adlandirilir */
#define wait   semafor_wait
#define signal semafor_signal

#define OKUR 4
#define YAZAR 2
#define TUR 2000

typedef sem_t semaphore;
static void wait(semaphore *s)   { sem_wait(s); }
static void signal(semaphore *s) { sem_post(s); }

semaphore turnike;    /* = 1 */
semaphore odaBos;     /* = 1: odada kimse yok */
semaphore mutex;      /* rc'yi korur = 1 */
int rc = 0;

static long veri = 0;
static atomic_int okuyan = 0, yazan = 0, ihlal = 0;
static atomic_long okuma = 0, yazma = 0;

static void yaz(void)
{
    if (atomic_fetch_add(&yazan, 1) != 0 || atomic_load(&okuyan) != 0)
        atomic_fetch_add(&ihlal, 1);
    veri++;
    atomic_fetch_add(&yazma, 1);
    atomic_fetch_sub(&yazan, 1);
}

static void oku(void)
{
    atomic_fetch_add(&okuyan, 1);
    if (atomic_load(&yazan) != 0)
        atomic_fetch_add(&ihlal, 1);
    long v = veri;
    (void)v;
    atomic_fetch_add(&okuma, 1);
    atomic_fetch_sub(&okuyan, 1);
}

static void *yazar(void *arg)
{
    (void)arg;
    for (int k = 0; k < TUR; k++) {
        wait(&turnike);
        wait(&odaBos);
          yaz();
        signal(&turnike);
        signal(&odaBos);
    }
    return NULL;
}

static void *okur(void *arg)
{
    (void)arg;
    for (int k = 0; k < TUR; k++) {
        wait(&turnike); signal(&turnike);
        wait(&mutex);
        if (++rc == 1) wait(&odaBos);     /* ilk okur odaBos'u alir */
        signal(&mutex);
          oku();
        wait(&mutex);
        if (--rc == 0) signal(&odaBos);   /* son okur birakir */
        signal(&mutex);
    }
    return NULL;
}

int main(void)
{
    pthread_t o[OKUR], y[YAZAR];
    sem_init(&turnike, 0, 1);
    sem_init(&odaBos, 0, 1);
    sem_init(&mutex, 0, 1);
    for (int i = 0; i < OKUR; i++)
        pthread_create(&o[i], NULL, okur, NULL);
    for (int i = 0; i < YAZAR; i++)
        pthread_create(&y[i], NULL, yazar, NULL);
    for (int i = 0; i < OKUR; i++)
        pthread_join(o[i], NULL);
    for (int i = 0; i < YAZAR; i++)
        pthread_join(y[i], NULL);

    printf("okuma = %ld, yazma = %ld, veri = %ld\n",
           atomic_load(&okuma), atomic_load(&yazma), veri);
    printf("kural ihlali = %d, rc = %d\n", atomic_load(&ihlal), rc);
    sem_destroy(&turnike);
    sem_destroy(&odaBos);
    sem_destroy(&mutex);
    return 0;
}

/* Beklenen cikti (her calistirmada ayni):
 * okuma = 8000, yazma = 4000, veri = 4000
 * kural ihlali = 0, rc = 0
 */
