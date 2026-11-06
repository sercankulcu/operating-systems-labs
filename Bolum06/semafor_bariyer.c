/* Semaforla bariyer (turnike): N = 3 is parcacigi bariyer()'e gelir; son gelen
 * turnikeyi acar, her gecen arkasindakini gecirir. down/up POSIX sem_wait/sem_post
 * ile yazilir. Hicbir is parcacigi, uc kisi de gelmeden bariyeri gecemez. */
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>

#define N 3

typedef sem_t semaphore;

static void down(semaphore *s) { sem_wait(s); }
static void up(semaphore *s)   { sem_post(s); }

semaphore mutex, turnike;           /* mutex = 1, turnike = 0 */
int sayac = 0;

static atomic_int gelen = 0;        /* bariyere ulasan */
static atomic_int erken_gecen = 0;  /* herkes gelmeden gecen (olmamali) */

void bariyer(void)
{
    down(&mutex);
    sayac = sayac + 1;
    if (sayac == N) up(&turnike);   /* son */
    up(&mutex);
    down(&turnike);                 /* turnikeden gec */
    up(&turnike);                   /* arkadakini gecir */
}

static void *isci(void *arg)
{
    (void)arg;
    atomic_fetch_add(&gelen, 1);
    bariyer();
    if (atomic_load(&gelen) != N)
        atomic_fetch_add(&erken_gecen, 1);
    return NULL;
}

int main(void)
{
    pthread_t t[N];
    sem_init(&mutex, 0, 1);
    sem_init(&turnike, 0, 0);
    for (int i = 0; i < N; i++)
        pthread_create(&t[i], NULL, isci, NULL);
    for (int i = 0; i < N; i++)
        pthread_join(t[i], NULL);

    int m, tk;
    sem_getvalue(&mutex, &m);
    sem_getvalue(&turnike, &tk);
    printf("mutex = %d, turnike = %d, sayac = %d\n", m, tk, sayac);
    printf("herkes gelmeden gecen: %d\n", atomic_load(&erken_gecen));
    sem_destroy(&mutex);
    sem_destroy(&turnike);
    return 0;
}

/* Beklenen cikti (her calistirmada ayni; turnike sonunda 1 kalir, bu yuzden
 * bu bariyer bir kez kullanilir, yeniden kullanim icin iki turnike gerekir):
 * mutex = 1, turnike = 1, sayac = 3
 * herkes gelmeden gecen: 0
 */
