/* Bellek modelleri ve yeniden siralama: "store buffer" turnusol testi.
 * x = y = 0;  T1: x = 1; r1 = y;   T2: y = 1; r2 = x;
 * Sirali tutarlilikta r1 == 0 && r2 == 0 imkansizdir; x86 bile bunu relaxed
 * erisimde uretebilir. seq_cst (bellek bariyeri) ile hic gorulmez. */
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdatomic.h>
#include <sched.h>

#define DENEME 50000

static atomic_int x, y;
static int r1, r2;                 /* her tur sonunda ana is parcacigi okur */
static bool bariyerli;

static atomic_int tur = -1;        /* ana is parcacigi turu baslatir */
static atomic_int biten = 0;       /* o turu bitiren is parcacigi sayisi */

static void bekle_tur(int k)
{
    unsigned donme = 0;
    while (atomic_load_explicit(&tur, memory_order_acquire) != k)
        if (++donme % 1024 == 0)
            sched_yield();
}

static void *T1(void *arg)
{
    (void)arg;
    for (int k = 0; k < DENEME; k++) {
        bekle_tur(k);
        if (bariyerli) {
            atomic_store_explicit(&x, 1, memory_order_seq_cst);
            r1 = atomic_load_explicit(&y, memory_order_seq_cst);
        } else {
            atomic_store_explicit(&x, 1, memory_order_relaxed);
            r1 = atomic_load_explicit(&y, memory_order_relaxed);
        }
        atomic_fetch_add_explicit(&biten, 1, memory_order_release);
    }
    return NULL;
}

static void *T2(void *arg)
{
    (void)arg;
    for (int k = 0; k < DENEME; k++) {
        bekle_tur(k);
        if (bariyerli) {
            atomic_store_explicit(&y, 1, memory_order_seq_cst);
            r2 = atomic_load_explicit(&x, memory_order_seq_cst);
        } else {
            atomic_store_explicit(&y, 1, memory_order_relaxed);
            r2 = atomic_load_explicit(&x, memory_order_relaxed);
        }
        atomic_fetch_add_explicit(&biten, 1, memory_order_release);
    }
    return NULL;
}

static int calistir(bool b)
{
    pthread_t a, c;
    int ikisi_sifir = 0;
    bariyerli = b;
    atomic_store(&tur, -1);
    atomic_store(&biten, 0);
    pthread_create(&a, NULL, T1, NULL);
    pthread_create(&c, NULL, T2, NULL);
    for (int k = 0; k < DENEME; k++) {
        atomic_store(&x, 0);                 /* x = y = 0 */
        atomic_store(&y, 0);
        atomic_store_explicit(&tur, k, memory_order_release);
        unsigned donme = 0;
        while (atomic_load_explicit(&biten, memory_order_acquire) != 2 * (k + 1))
            if (++donme % 1024 == 0)
                sched_yield();
        if (r1 == 0 && r2 == 0)              /* r1 == 0 && r2 == 0 ? */
            ikisi_sifir++;
    }
    pthread_join(a, NULL);
    pthread_join(c, NULL);
    return ikisi_sifir;
}

int main(void)
{
    printf("%d denemede r1 == 0 && r2 == 0:\n", DENEME);
    printf("relaxed (bariyersiz): %d kez\n", calistir(false));
    printf("seq_cst (bariyerli) : %d kez\n", calistir(true));
    return 0;
}

/* Beklenen cikti (x86-64; ilk sayi her calistirmada degisir, 0 da olabilir;
 * ikinci sayi her zaman 0):
 * 50000 denemede r1 == 0 && r2 == 0:
 * relaxed (bariyersiz): N kez
 * seq_cst (bariyerli) : 0 kez
 */
