/* Neden if degil while? Mesa anlaminda signal yalniz "ipucu"dur: uyanan is parcacigi
 * kilidi yeniden alana kadar baska biri (B) ogeyi kapabilir. Tuketici A dolu'da
 * bekler; ana is parcacigi koy(), signal ve hemen ardindan B olarak al() yapar.
 * if surumunde A bos tampondan okur; while surumunde yeniden bekler. */
#include <stdio.h>
#include <pthread.h>
#include <time.h>

static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t dolu = PTHREAD_COND_INITIALIZER;
static int count = 0;            /* tampondaki oge */
static int sonraki = 0;
static int bekleyen = 0, uyanma = 0;

static void lock(pthread_mutex_t *x)   { pthread_mutex_lock(x); }
static void unlock(pthread_mutex_t *x) { pthread_mutex_unlock(x); }
static void cond_wait(pthread_cond_t *c, pthread_mutex_t *x)
{
    bekleyen++;
    pthread_cond_wait(c, x);
    bekleyen--;
    uyanma++;
}

static void koy(void) { count++; sonraki++; }
static int al(void)                     /* bos tampondan almak HATA'dir */
{
    if (count == 0)
        return -1;
    count--;
    return sonraki;
}

static int A_sonuc;

/* YANLIS (Mesa'da) */
static void *tuketici_if(void *arg)
{
    int x;
    (void)arg;
    lock(&m);
    if (count == 0)
        cond_wait(&dolu, &m);
    x = al();        /* bos olabilir */
    unlock(&m);
    A_sonuc = x;
    return NULL;
}

/* DOGRU */
static void *tuketici_while(void *arg)
{
    int x;
    (void)arg;
    lock(&m);
    while (count == 0)
        cond_wait(&dolu, &m);
    x = al();        /* count > 0 kesin */
    unlock(&m);
    A_sonuc = x;
    return NULL;
}

/* A, uyanma sayisi u olacak sekilde dolu'da bekleyene kadar bekle */
static void bekle_A(int u)
{
    for (;;) {
        lock(&m);
        int hazir = (bekleyen == 1 && uyanma == u);
        unlock(&m);
        if (hazir)
            return;
        struct timespec ts = { 0, 1000000 };
        nanosleep(&ts, NULL);
    }
}

static void senaryo(const char *ad, void *(*tuketici)(void *), int ikinci_koy)
{
    pthread_t A;
    count = 0; sonraki = 0; bekleyen = 0; uyanma = 0;
    pthread_create(&A, NULL, tuketici, NULL);
    bekle_A(0);                       /* A dolu'da uyuyor */

    lock(&m);
    koy();                            /* uretici: oge 1 */
    pthread_cond_signal(&dolu);       /* A uyandirilir ama kilidi henuz alamaz */
    int B = al();                     /* B araya girip ogeyi kapar */
    unlock(&m);
    printf("%s: B al() = %d\n", ad, B);

    if (ikinci_koy) {                 /* while: A yeniden bekliyor */
        bekle_A(1);
        printf("%s: A uyandi, count = 0 gordu, yeniden bekliyor\n", ad);
        lock(&m);
        koy();                        /* oge 2 */
        pthread_cond_signal(&dolu);
        unlock(&m);
    }
    pthread_join(A, NULL);
    if (A_sonuc < 0)
        printf("%s: A al() = %d -> bos tampondan okudu (HATA)\n", ad, A_sonuc);
    else
        printf("%s: A al() = %d -> dogru\n", ad, A_sonuc);
}

int main(void)
{
    senaryo("if   ", tuketici_if, 0);
    senaryo("while", tuketici_while, 1);
    return 0;
}

/* Beklenen cikti (her calistirmada ayni; cok nadir sahte uyanma "if" surumunde
 * A'yi erken uyandirabilir):
 * if   : B al() = 1
 * if   : A al() = -1 -> bos tampondan okudu (HATA)
 * while: B al() = 1
 * while: A uyandi, count = 0 gordu, yeniden bekliyor
 * while: A al() = 2 -> dogru
 */
