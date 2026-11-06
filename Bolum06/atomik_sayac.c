/* Atomik degiskenler (C11 <stdatomic.h>): atomic_fetch_add kilitsiz ve dogru.
 * Karsilastirma icin ayni dongu duz int ile de calistirilir (yaris durumu). */
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

atomic_int sayac = 0;     /* _Atomic int */
static volatile int duz = 0;   /* korumasiz: sayac++ uc komuttur */

void *is(void *arg)
{
    (void)arg;
    for (int n = 0; n < 100000; n++)
        atomic_fetch_add(&sayac, 1);
    return NULL;
}

static void *duz_is(void *arg)
{
    (void)arg;
    for (int n = 0; n < 100000; n++)
        duz = duz + 1;
    return NULL;
}

int main(void)
{
    pthread_t t[2];
    for (int i = 0; i < 2; i++)
        pthread_create(&t[i], NULL, is, NULL);
    for (int i = 0; i < 2; i++)
        pthread_join(t[i], NULL);
    printf("atomic_fetch_add: %d\n", atomic_load(&sayac));   /* 2 is parcacigi + join -> 200000 */

    for (int i = 0; i < 2; i++)
        pthread_create(&t[i], NULL, duz_is, NULL);
    for (int i = 0; i < 2; i++)
        pthread_join(t[i], NULL);
    printf("duz int         : %d\n", duz);
    return 0;
}

/* Beklenen cikti (ilk satir her zaman 200000; ikinci satir yaris durumu
 * yuzunden genellikle 200000'den kucuktur ve her calistirmada degisir):
 * atomic_fetch_add: 200000
 * duz int         : 1xxxxx
 */
