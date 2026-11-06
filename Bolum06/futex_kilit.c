/* Linux futex ile mutex (Drepper, "Futexes Are Tricky"): kilit sozcugu
 * 0 = bos, 1 = dolu, 2 = dolu ve bekleyen var. Cekisme yoksa tek CAS yeter,
 * cekirdege girilmez; doluysa futex_wait ile uyunur, birakan futex_wake cagirir. */
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/futex.h>

#define IS_PARCACIGI 4
#define TUR 100000

static atomic_int kilit_sozcugu = 0;
static atomic_long wait_sayisi = 0, wake_sayisi = 0;

static long futex(atomic_int *adres, int op, int deger)
{
    return syscall(SYS_futex, (int *)adres, op, deger, NULL, NULL, 0);
}

static void futex_lock(atomic_int *w)
{
    int c = 0;
    if (atomic_compare_exchange_strong(w, &c, 1))
        return;                              /* CAS 0 -> 1: sistem cagrisi yok */
    if (c != 2)
        c = atomic_exchange(w, 2);           /* "bekleyen var" isaretle */
    while (c != 0) {
        atomic_fetch_add_explicit(&wait_sayisi, 1, memory_order_relaxed);
        futex(w, FUTEX_WAIT_PRIVATE, 2);     /* deger hala 2 ise uyu */
        c = atomic_exchange(w, 2);           /* uyandi: eski 0 ise kilit alindi */
    }
}

static void futex_unlock(atomic_int *w)
{
    if (atomic_fetch_sub(w, 1) != 1) {       /* eski deger 2: bekleyen var */
        atomic_store(w, 0);
        atomic_fetch_add_explicit(&wake_sayisi, 1, memory_order_relaxed);
        futex(w, FUTEX_WAKE_PRIVATE, 1);     /* birini uyandir */
    }
}

static long sayac = 0;

static void *isci(void *arg)
{
    (void)arg;
    for (int k = 0; k < TUR; k++) {
        futex_lock(&kilit_sozcugu);
        sayac++;
        futex_unlock(&kilit_sozcugu);
    }
    return NULL;
}

int main(void)
{
    /* cekisme yok: tek is parcacigi, hic futex cagrisi olmaz */
    futex_lock(&kilit_sozcugu);
    futex_unlock(&kilit_sozcugu);
    printf("cekismesiz: futex_wait = %ld, futex_wake = %ld\n",
           atomic_load(&wait_sayisi), atomic_load(&wake_sayisi));

    pthread_t t[IS_PARCACIGI];
    for (int i = 0; i < IS_PARCACIGI; i++)
        pthread_create(&t[i], NULL, isci, NULL);
    for (int i = 0; i < IS_PARCACIGI; i++)
        pthread_join(t[i], NULL);
    printf("sayac = %ld\n", sayac);
    printf("cekismeli: futex_wait = %ld, futex_wake = %ld (degisir)\n",
           atomic_load(&wait_sayisi), atomic_load(&wake_sayisi));
    return 0;
}

/* Beklenen cikti (son satirdaki sayilar zamanlamaya bagli):
 * cekismesiz: futex_wait = 0, futex_wake = 0
 * sayac = 400000
 * cekismeli: futex_wait = N, futex_wake = M (degisir)
 */
