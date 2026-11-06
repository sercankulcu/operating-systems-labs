/* Mutex gerceklemesi (Tanenbaum Sekil 2-29): TSL ile kilidi dene, doluysa
 * donmek yerine thread_yield ile CPU'yu birak. Dort is parcacigi ayni mutex'le
 * sayac artirir; thread_yield cagrisi sayisi da sayilir. */
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <sched.h>

#define IS_PARCACIGI 4
#define TUR 50000

static atomic_int MUTEX = 0;
static atomic_long yield_sayisi = 0;

static void thread_yield(void)
{
    atomic_fetch_add_explicit(&yield_sayisi, 1, memory_order_relaxed);
    sched_yield();
}

void mutex_lock(void)
{
    for (;;) {
        int REGISTER = atomic_exchange(&MUTEX, 1);  /* TSL REGISTER,MUTEX */
        if (REGISTER == 0)                          /* CMP REGISTER,#0 / JZE ok */
            return;                                 /* ok: RET */
        thread_yield();                             /* CALL thread_yield */
    }                                               /* JMP mutex_lock */
}

void mutex_unlock(void)
{
    atomic_store(&MUTEX, 0);                        /* MOVE MUTEX,#0 */
}

static long sayac = 0;

static void *isci(void *arg)
{
    (void)arg;
    for (int k = 0; k < TUR; k++) {
        mutex_lock();
        sayac++;
        mutex_unlock();
    }
    return NULL;
}

int main(void)
{
    pthread_t t[IS_PARCACIGI];
    for (int i = 0; i < IS_PARCACIGI; i++)
        pthread_create(&t[i], NULL, isci, NULL);
    for (int i = 0; i < IS_PARCACIGI; i++)
        pthread_join(t[i], NULL);
    printf("sayac = %ld\n", sayac);
    printf("thread_yield cagrisi = %ld (degisir)\n", atomic_load(&yield_sayisi));
    return 0;
}

/* Beklenen cikti (ikinci sayi zamanlamaya bagli olarak degisir):
 * sayac = 200000
 * thread_yield cagrisi = N (degisir)
 */
