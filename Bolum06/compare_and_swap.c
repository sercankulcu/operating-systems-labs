/* compare_and_swap ile kilit: "eski == bekl ise yeni yaz, eskiyi dondur" tek atomik
 * komuttur. Once slayttaki iz (T0 alir, T1 doner, T0 birakir, T1 alir) tek is
 * parcacigiyla oynatilir, sonra iki is parcacigi kilitle sayac artirir. */
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <sched.h>

#define TUR 100000

/* Slayttaki govde (uc satir birlikte TEK atomik komut):
 *     int eski = *v;  if (eski == bekl) *v = yeni;  return eski;
 * C11 karsiligi: atomic_compare_exchange_strong (x86: lock cmpxchg).
 * Basarisiz olursa bekl'e guncel deger yazilir; yani her durumda eski degeri verir. */
int compare_and_swap(atomic_int *v, int bekl, int yeni)
{
    int eski = bekl;
    (void)atomic_compare_exchange_strong(v, &eski, yeni);
    return eski;
}

atomic_int lock = 0;

void gir(void)
{
    unsigned donme = 0;
    while (compare_and_swap(&lock, 0, 1) != 0)
        if (++donme % 1024 == 0)      /* mesgul bekle */
            sched_yield();
}

void cik(void)
{
    atomic_store(&lock, 0);
}

static long sayac = 0;

static void *isci(void *arg)
{
    (void)arg;
    for (int k = 0; k < TUR; k++) {
        gir();
        sayac++;                      /* kritik bolge */
        cik();
    }
    return NULL;
}

int main(void)
{
    /* Slayttaki iz */
    int eski;
    printf("lock = %d\n", atomic_load(&lock));
    eski = compare_and_swap(&lock, 0, 1);
    printf("T0: eski = %d -> kritik bolgeye girdi, lock = %d\n", eski, atomic_load(&lock));
    eski = compare_and_swap(&lock, 0, 1);
    printf("T1: eski = %d -> doner (mesgul bekler), lock = %d\n", eski, atomic_load(&lock));
    cik();
    printf("T0: cik(), lock = %d\n", atomic_load(&lock));
    eski = compare_and_swap(&lock, 0, 1);
    printf("T1: eski = %d -> kritik bolgeye girdi, lock = %d\n", eski, atomic_load(&lock));
    cik();

    /* Gercek is parcaciklariyla */
    pthread_t t[2];
    for (int i = 0; i < 2; i++)
        pthread_create(&t[i], NULL, isci, NULL);
    for (int i = 0; i < 2; i++)
        pthread_join(t[i], NULL);
    printf("iki is parcacigi x %d: sayac = %ld\n", TUR, sayac);
    return 0;
}

/* Beklenen cikti (her calistirmada ayni):
 * lock = 0
 * T0: eski = 0 -> kritik bolgeye girdi, lock = 1
 * T1: eski = 1 -> doner (mesgul bekler), lock = 1
 * T0: cik(), lock = 0
 * T1: eski = 0 -> kritik bolgeye girdi, lock = 1
 * iki is parcacigi x 100000: sayac = 200000
 */
