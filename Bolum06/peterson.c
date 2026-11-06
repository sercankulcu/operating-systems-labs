/* Peterson algoritmasi: iki surec icin yalniz okuma/yazma ile karsilikli dislama.
 * flag ve turn C11 atomik (varsayilan seq_cst) tanimlanir; boylece modern CPU'daki
 * yeniden siralama engellenir ve algoritma dogru calisir. */
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdatomic.h>
#include <sched.h>

#define TUR 100000

/* paylasilan */
atomic_bool flag[2] = { false, false };
atomic_int  turn;

static long sayac = 0;                /* kritik bolgede korunan veri */

void gir(int i)
{
    int j = 1 - i;
    atomic_store(&flag[i], true);     /* istiyorum */
    atomic_store(&turn, j);           /* sen buyur */
    unsigned donme = 0;
    while (atomic_load(&flag[j]) && atomic_load(&turn) == j) {
        if (++donme % 1024 == 0)      /* mesgul bekle; az cekirdekte CPU'yu birak */
            sched_yield();
    }
}

void cik(int i)
{
    atomic_store(&flag[i], false);
}

static void *P(void *arg)
{
    int i = *(int *)arg;              /* P_i  (j = 1 - i) */
    for (int k = 0; k < TUR; k++) {   /* while (true) yerine TUR tur */
        gir(i);
        sayac++;                      /* kritik bolge */
        cik(i);
        /* kalan bolum */
    }
    return NULL;
}

int main(void)
{
    pthread_t t[2];
    int id[2] = { 0, 1 };
    atomic_init(&turn, 0);
    for (int i = 0; i < 2; i++)
        pthread_create(&t[i], NULL, P, &id[i]);
    for (int i = 0; i < 2; i++)
        pthread_join(t[i], NULL);
    printf("sayac = %ld\n", sayac);
    return 0;
}

/* Beklenen cikti (her calistirmada ayni):
 * sayac = 200000
 */
