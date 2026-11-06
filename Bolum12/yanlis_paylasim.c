/* Yanlis paylasim (false sharing): iki is parcacigi bagimsiz iki sayaci
 * artirir. Sayaclar ayni 64 baytlik onbellek satirindaysa satir cekirdekler
 * arasinda gidip gelir; _Alignas(64) ile ayri satirlara konunca hizlanir. */
#include <stdio.h>
#include <stddef.h>
#include <pthread.h>
#include <time.h>

#define TUR 20000000L

struct sayaclar {      /* kotu */
    long a;   /* bagil konum 0 */
    long b;   /* bagil konum 8 */
};

struct dolgulu {       /* iyi */
    _Alignas(64) long a;
    _Alignas(64) long b;
};

static volatile struct sayaclar s;     /* volatile: derleyici dongu */
static volatile struct dolgulu d;      /* icinde artirmayi silmesin */

static void *s_a(void *x) { (void)x; for (long i = 0; i < TUR; i++) s.a++; return NULL; }
static void *s_b(void *x) { (void)x; for (long i = 0; i < TUR; i++) s.b++; return NULL; }
static void *d_a(void *x) { (void)x; for (long i = 0; i < TUR; i++) d.a++; return NULL; }
static void *d_b(void *x) { (void)x; for (long i = 0; i < TUR; i++) d.b++; return NULL; }

static double calistir(void *(*f0)(void *), void *(*f1)(void *))
{
    struct timespec t0, t1;
    pthread_t p0, p1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    pthread_create(&p0, NULL, f0, NULL);   /* is parcacigi 0: for(..) s.a++; */
    pthread_create(&p1, NULL, f1, NULL);   /* is parcacigi 1: for(..) s.b++; */
    pthread_join(p0, NULL);
    pthread_join(p1, NULL);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    return (double)(t1.tv_sec - t0.tv_sec) * 1e3 + (double)(t1.tv_nsec - t0.tv_nsec) / 1e6;
}

int main(void)
{
    printf("sayaclar: b'nin bagil konumu %zu, boyut %zu\n",
           offsetof(struct sayaclar, b), sizeof(struct sayaclar));
    printf("dolgulu:  b'nin bagil konumu %zu, boyut %zu\n",
           offsetof(struct dolgulu, b), sizeof(struct dolgulu));

    double kotu = calistir(s_a, s_b);
    double iyi = calistir(d_a, d_b);
    printf("ayni satir:  %.1f ms (a = %ld, b = %ld)\n", kotu, s.a, s.b);
    printf("ayri satir:  %.1f ms (a = %ld, b = %ld)\n", iyi, d.a, d.b);
    return 0;
}

/* Beklenen cikti (sureler makineye gore degisir; cok cekirdekli makinede
 * ayni satir genellikle birkac kat yavastir):
 * sayaclar: b'nin bagil konumu 8, boyut 16
 * dolgulu:  b'nin bagil konumu 64, boyut 128
 * ayni satir:  180.0 ms (a = 20000000, b = 20000000)
 * ayri satir:  45.0 ms (a = 20000000, b = 20000000)
 */
