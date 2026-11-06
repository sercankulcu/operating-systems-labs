/* OpenMP'nin iki yonergesi pthreads ile elle yazilir:
 *   #pragma omp parallel                       -> her is parcacigi bolgeyi calistirir
 *   #pragma omp parallel for reduction(+:s)    -> dongu bolunur, kismi toplamlar birlesir
 * Gercek OpenMP surumu "gcc -fopenmp" ister; CI bu bayragi kullanmadigi icin
 * omp_get_thread_num() burada _Thread_local bir degiskenle taklit edilir.
 *
 * Asil kod:
 *   #include <omp.h>
 *   #pragma omp parallel
 *   printf("merhaba, ben %d\n", omp_get_thread_num());
 *   #pragma omp parallel for reduction(+:s)
 *   for (int i = 0; i < 1000; i++) { c[i] = a[i] + b[i]; s += c[i]; }
 */
#include <stdio.h>
#include <pthread.h>

#define OMP_NUM_THREADS 4

static _Thread_local int ben_no;
static int omp_get_thread_num(void) { return ben_no; }

double a[1000], b[1000], c[1000], s = 0;
static double kismi_s[OMP_NUM_THREADS];          /* reduction icin ozel kopyalar */
static char merhaba[OMP_NUM_THREADS][32];

/* #pragma omp parallel bolgesi */
static void *paralel_bolge(void *arg)
{
    ben_no = (int)(long)arg;
    snprintf(merhaba[ben_no], sizeof merhaba[ben_no],
             "merhaba, ben %d", omp_get_thread_num());
    return NULL;
}

/* #pragma omp parallel for reduction(+:s): statik bolumleme */
static void *paralel_for(void *arg)
{
    ben_no = (int)(long)arg;
    int parca = 1000 / OMP_NUM_THREADS;
    int bas = ben_no * parca, son = bas + parca;
    double s_ozel = 0;                            /* her is parcacigina ozel s */
    for (int i = bas; i < son; i++) {
        c[i] = a[i] + b[i];
        s_ozel += c[i];
    }
    kismi_s[omp_get_thread_num()] = s_ozel;
    return NULL;
}

static void paralel_calistir(void *(*f)(void *))
{
    pthread_t t[OMP_NUM_THREADS];
    for (long k = 0; k < OMP_NUM_THREADS; k++)
        pthread_create(&t[k], NULL, f, (void *)k);
    for (int k = 0; k < OMP_NUM_THREADS; k++)
        pthread_join(t[k], NULL);
}

int main(void)
{
    paralel_calistir(paralel_bolge);
    for (int k = 0; k < OMP_NUM_THREADS; k++)     /* sirali yazdir */
        printf("%s\n", merhaba[k]);

    /* ... a ve b doldurulur ... */
    for (int i = 0; i < 1000; i++) {
        a[i] = i;
        b[i] = 2 * i;
    }
    paralel_calistir(paralel_for);
    for (int k = 0; k < OMP_NUM_THREADS; k++)     /* reduction: birlestir */
        s += kismi_s[k];
    printf("s = %.0f\n", s);
    return 0;
}

/* Beklenen cikti (gercek OpenMP'de "merhaba" satirlarinin sirasi degisir;
 * burada join'den sonra sirali yazdirilir):
 * merhaba, ben 0
 * merhaba, ben 1
 * merhaba, ben 2
 * merhaba, ben 3
 * s = 1498500
 */
