/* Kimi uyandirmali? signal ve broadcast. bellek_al(n) yeterli bos bayt yoksa
 * bekler. bos = 0 iken T1 100, T2 10 bayt ister ve ikisi de bekler. bellek_ver(50)
 * signal ile yalniz T1'i uyandirsaydi T1 yine bekleyecek, T2 (karsilanabilir)
 * uyumaya devam edecekti. broadcast ikisini de uyandirir, her biri kendi kosulunu
 * yeniden sinar: T2 ilerler, T1 bekler. */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t c = PTHREAD_COND_INITIALIZER;

static void lock(pthread_mutex_t *x)   { pthread_mutex_lock(x); }
static void unlock(pthread_mutex_t *x) { pthread_mutex_unlock(x); }
static void broadcast(pthread_cond_t *k) { pthread_cond_broadcast(k); }

static int bekleyen = 0;
static void wait_c(pthread_cond_t *k, pthread_mutex_t *x)
{
    bekleyen++;
    pthread_cond_wait(k, x);
    bekleyen--;
}
#define wait wait_c                /* <sys/wait.h> wait() ile karismasin */

int bos = 0;            /* bos bayt */

void *bellek_al(int n)
{
    lock(&m);
    while (bos < n)
        wait(&c, &m);
    bos -= n;  /* ... ayir */
    unlock(&m);
    return malloc((size_t)n);      /* slaytta return eksikti */
}

void bellek_ver(int n)
{
    lock(&m);
    bos += n;
    broadcast(&c); /* signal yetmez */
    unlock(&m);
}

static void *T(void *arg)
{
    int n = *(int *)arg;
    void *p = bellek_al(n);
    free(p);
    return NULL;
}

static void bekleyen_bekle(int k)
{
    for (;;) {
        lock(&m);
        int b = bekleyen;
        unlock(&m);
        if (b == k)
            return;
        struct timespec ts = { 0, 1000000 };
        nanosleep(&ts, NULL);
    }
}

static int bos_oku(void)
{
    lock(&m);
    int b = bos;
    unlock(&m);
    return b;
}

int main(void)
{
    pthread_t t1, t2;
    int n1 = 100, n2 = 10;
    pthread_create(&t1, NULL, T, &n1);
    bekleyen_bekle(1);
    pthread_create(&t2, NULL, T, &n2);
    bekleyen_bekle(2);
    printf("bos = %d: T1 (100) ve T2 (10) bekliyor\n", bos_oku());

    bellek_ver(50);
    pthread_join(t2, NULL);             /* broadcast sayesinde T2 ilerler */
    bekleyen_bekle(1);                  /* T1 yeniden uyudu */
    printf("bellek_ver(50) -> T2 10 bayt aldi, T1 hala bekliyor, bos = %d\n", bos_oku());

    bellek_ver(60);
    pthread_join(t1, NULL);
    printf("bellek_ver(60) -> T1 100 bayt aldi, bos = %d\n", bos_oku());
    return 0;
}

/* Beklenen cikti (her calistirmada ayni):
 * bos = 0: T1 (100) ve T2 (10) bekliyor
 * bellek_ver(50) -> T2 10 bayt aldi, T1 hala bekliyor, bos = 40
 * bellek_ver(60) -> T1 100 bayt aldi, bos = 0
 */
