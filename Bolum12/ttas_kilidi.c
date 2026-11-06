/* TTAS (test-and-test-and-set) kilidi: bekleyen once kilidi yalnizca okur
 * (kendi onbellek kopyasinda doner), serbest gorunce TSL dener. tsl() C11
 * atomic_exchange ile gerceklenir. Uc is parcacigi ortak sayaci artirir. */
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

#define NTHR 3
#define TUR 100000

static int tsl(atomic_int *k)
{
    return atomic_exchange(k, 1);     /* eski degeri dondur, 1 yaz */
}

void kilitle(atomic_int *k)
{
    for (;;) {
        while (*k == 1)   /* yerelde don */
            ;
        if (tsl(k) == 0)  /* serbest gorundu */
            return;       /* kilit alindi */
    }
}

void ac(atomic_int *k)
{
    *k = 0;
}

static atomic_int kilit = 0;
static long sayac = 0;

static void *is(void *arg)
{
    (void)arg;
    for (int i = 0; i < TUR; i++) {
        kilitle(&kilit);
        sayac++;
        ac(&kilit);
    }
    return NULL;
}

int main(void)
{
    pthread_t t[NTHR];
    for (int i = 0; i < NTHR; i++)
        pthread_create(&t[i], NULL, is, NULL);
    for (int i = 0; i < NTHR; i++)
        pthread_join(t[i], NULL);
    printf("sayac = %ld (beklenen %d)\n", sayac, NTHR * TUR);
    return 0;
}

/* Beklenen cikti:
 * sayac = 300000 (beklenen 300000)
 */
