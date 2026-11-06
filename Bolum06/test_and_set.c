/* Atomik donanim komutlari: test_and_set ile basit spinlock, ve TSL / XCHG
 * komutlariyla yazilan enter_region / leave_region'in C11 karsiligi.
 * Iki is parcacigi 100000'er kez kritik bolgede sayac artirir. */
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdatomic.h>
#include <sched.h>

#define TUR 100000

/* Slayttaki tanim (tumu TEK atomik komuttur):
 *     bool eski = *hedef;  *hedef = true;  return eski;
 * C11'de bu tek komut atomic_exchange'dir (x86: xchg, ARM: ldaxr/stlxr ya da swp). */
bool test_and_set(atomic_bool *hedef)
{
    return atomic_exchange(hedef, true);
}

/* kullanim: basit spinlock */
atomic_bool lock = false;

static long sayac1 = 0;

static void *spin_isci(void *arg)
{
    (void)arg;
    for (int k = 0; k < TUR; k++) {
        unsigned donme = 0;
        while (test_and_set(&lock))
            if (++donme % 1024 == 0)     /* mesgul bekle */
                sched_yield();
        sayac1++;                        /* kritik bolge */
        atomic_store(&lock, false);      /* lock = false; */
    }
    return NULL;
}

/* TSL REGISTER,LOCK ya da MOVE REGISTER,#1 + XCHG REGISTER,LOCK:
 * yazmac = LOCK; LOCK = 1 tek komutta. CMP REGISTER,#0; JNE enter_region. */
static atomic_int LOCK = 0;
static long sayac2 = 0;

void enter_region(void)
{
    int REGISTER;
    unsigned donme = 0;
    do {
        REGISTER = atomic_exchange(&LOCK, 1);   /* XCHG REGISTER,LOCK */
        if (REGISTER != 0 && ++donme % 1024 == 0)
            sched_yield();
    } while (REGISTER != 0);                    /* LOCK 0 miydi? degilse tekrar */
}                                               /* kritik bolgeye girildi */

void leave_region(void)
{
    atomic_store(&LOCK, 0);                     /* MOVE LOCK,#0 */
}

static void *tsl_isci(void *arg)
{
    (void)arg;
    for (int k = 0; k < TUR; k++) {
        enter_region();
        sayac2++;
        leave_region();
    }
    return NULL;
}

static void iki_isci(void *(*f)(void *))
{
    pthread_t t[2];
    for (int i = 0; i < 2; i++)
        pthread_create(&t[i], NULL, f, NULL);
    for (int i = 0; i < 2; i++)
        pthread_join(t[i], NULL);
}

int main(void)
{
    iki_isci(spin_isci);
    printf("test_and_set spinlock: sayac = %ld\n", sayac1);
    iki_isci(tsl_isci);
    printf("enter_region (XCHG)  : sayac = %ld\n", sayac2);
    return 0;
}

/* Beklenen cikti (her calistirmada ayni):
 * test_and_set spinlock: sayac = 200000
 * enter_region (XCHG)  : sayac = 200000
 */
