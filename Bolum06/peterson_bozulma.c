/* Peterson modern CPU'da neden bozulur: her cekirdegin yazma tamponu (store buffer)
 * "flag[i] = true" yazisini geciktirir, sonraki "flag[j]" okumasi once yapilir.
 * Iki surec de flag[j] == false gorup birlikte kritik bolgeye girer.
 * Ayni kod once relaxed (siralamasiz), sonra seq_cst (bellek bariyerli) calisir. */
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdatomic.h>
#include <sched.h>

#define TUR 200000

static atomic_bool flag[2];
static atomic_int  turn;
static bool bariyerli;                     /* false: relaxed, true: seq_cst */

static atomic_int icerideki;
static atomic_int ihlal;

/* Bellek sirasi derleme aninda sabit olmali (degiskense GCC seq_cst sayar);
 * bu yuzden iki surum ayri ayri yazilir. */
static void gir_relaxed(int i)
{
    int j = 1 - i;
    unsigned donme = 0;
    atomic_store_explicit(&flag[i], true, memory_order_relaxed);
    atomic_store_explicit(&turn, j, memory_order_relaxed);
    while (atomic_load_explicit(&flag[j], memory_order_relaxed)
           && atomic_load_explicit(&turn, memory_order_relaxed) == j) {
        if (++donme % 1024 == 0)
            sched_yield();
    }
}

static void gir_seq_cst(int i)
{
    int j = 1 - i;
    unsigned donme = 0;
    atomic_store_explicit(&flag[i], true, memory_order_seq_cst);
    atomic_store_explicit(&turn, j, memory_order_seq_cst);
    while (atomic_load_explicit(&flag[j], memory_order_seq_cst)
           && atomic_load_explicit(&turn, memory_order_seq_cst) == j) {
        if (++donme % 1024 == 0)
            sched_yield();
    }
}

static void gir(int i)
{
    if (bariyerli)
        gir_seq_cst(i);
    else
        gir_relaxed(i);
}

static void cik(int i)
{
    if (bariyerli)
        atomic_store_explicit(&flag[i], false, memory_order_seq_cst);
    else
        atomic_store_explicit(&flag[i], false, memory_order_relaxed);
}

static void *P(void *arg)
{
    int i = *(int *)arg;
    for (int k = 0; k < TUR; k++) {
        gir(i);
        /* kritik bolge: iceride baska biri var mi? */
        if (atomic_fetch_add_explicit(&icerideki, 1, memory_order_relaxed) != 0)
            atomic_fetch_add_explicit(&ihlal, 1, memory_order_relaxed);
        atomic_fetch_sub_explicit(&icerideki, 1, memory_order_relaxed);
        cik(i);
    }
    return NULL;
}

static int calistir(bool b)
{
    pthread_t t[2];
    int id[2] = { 0, 1 };
    bariyerli = b;
    atomic_store(&flag[0], false);
    atomic_store(&flag[1], false);
    atomic_store(&turn, 0);
    atomic_store(&icerideki, 0);
    atomic_store(&ihlal, 0);
    for (int i = 0; i < 2; i++)
        pthread_create(&t[i], NULL, P, &id[i]);
    for (int i = 0; i < 2; i++)
        pthread_join(t[i], NULL);
    return atomic_load(&ihlal);
}

int main(void)
{
    int r = calistir(false);
    printf("relaxed (bariyersiz): %d ihlal\n", r);
    int s = calistir(true);
    printf("seq_cst (bariyerli) : %d ihlal\n", s);
    return 0;
}

/* Beklenen cikti (x86-64, cok cekirdek; ilk sayi her calistirmada degisir ve
 * sansla 0 da olabilir, ikinci satir her zaman 0'dir):
 * relaxed (bariyersiz): N ihlal
 * seq_cst (bariyerli) : 0 ihlal
 */
