/* Yemek yiyen filozoflar, naif cozum: her filozof once sol, sonra sag catali alir.
 * Bes filozof ayni anda sol catali alirsa hepsi sag catali bekler: kilitlenme.
 * Bu kotu sirayi bir gecikme zorlar (herkes sol catali aldiktan sonra 100 ms
 * dusunur); sag catal 1 s icinde alinamazsa filozof sol catali birakip vazgecer. */
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <time.h>
#include <errno.h>

#define wait   semafor_wait      /* <sys/wait.h> ile karismasin */
#define signal semafor_signal    /* <signal.h> ile karismasin */

#define TUR 3

typedef sem_t semaphore;

static int wait(semaphore *s)    /* 1 s icinde alinamazsa 0 */
{
    struct timespec son;
    clock_gettime(CLOCK_REALTIME, &son);
    son.tv_sec += 1;
    while (sem_timedwait(s, &son) != 0)
        if (errno == ETIMEDOUT)
            return 0;
    return 1;
}
static void signal(semaphore *s) { sem_post(s); }

semaphore catal[5];        /* hepsi 1 */

static atomic_int yemek = 0;
static int takildi[5];     /* sag catali bekleyip vazgecti mi */
static atomic_int takilan = 0;

static void dusun(void) { }
static void ye(void) { atomic_fetch_add(&yemek, 1); }

static void kotu_zamanlama(void)   /* sol catal alindiktan sonra kesilme */
{
    struct timespec ts = { 0, 100000000 };   /* 100 ms */
    nanosleep(&ts, NULL);
}

void filozof(int i)
{
    for (int k = 0; k < TUR; k++) {          /* while (true) yerine TUR tur */
        dusun();
        wait(&catal[i]);                     /* sol */
        kotu_zamanlama();
        if (!wait(&catal[(i + 1) % 5])) {    /* sag */
            takildi[i] = 1;
            atomic_fetch_add(&takilan, 1);
            /* gercekte sonsuza dek bekler. Sol catali hemen birakirsak henuz
             * zaman asimina ugramamis komsu onu alip yiyebilir; bu yuzden
             * herkes takilana (en fazla 2 s) kadar catal elde tutulur. */
            for (int d = 0; d < 2000 && atomic_load(&takilan) < 5; d++) {
                struct timespec ts = { 0, 1000000 };
                nanosleep(&ts, NULL);
            }
            signal(&catal[i]);
            return;
        }
        ye();
        signal(&catal[(i + 1) % 5]);
        signal(&catal[i]);
    }
}

static void *filozof_is(void *arg)
{
    filozof(*(int *)arg);
    return NULL;
}

int main(void)
{
    pthread_t t[5];
    int id[5];
    for (int i = 0; i < 5; i++)
        sem_init(&catal[i], 0, 1);
    for (int i = 0; i < 5; i++) {
        id[i] = i;
        pthread_create(&t[i], NULL, filozof_is, &id[i]);
    }
    for (int i = 0; i < 5; i++)
        pthread_join(t[i], NULL);

    int n = 0;
    for (int i = 0; i < 5; i++)
        n += takildi[i];
    printf("yenen yemek = %d\n", atomic_load(&yemek));
    printf("sol catali tutup sagi bekleyen filozof = %d\n", n);
    if (n == 5)
        printf("kilitlenme: herkes solundakini tutuyor, kimse yiyemiyor\n");
    for (int i = 0; i < 5; i++)
        sem_destroy(&catal[i]);
    return 0;
}

/* Beklenen cikti (gecikme kotu sirayi zorladigi icin pratikte her calistirmada
 * ayni; yaklasik 1.1 s surer):
 * yenen yemek = 0
 * sol catali tutup sagi bekleyen filozof = 5
 * kilitlenme: herkes solundakini tutuyor, kimse yiyemiyor
 */
