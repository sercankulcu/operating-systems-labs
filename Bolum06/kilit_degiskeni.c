/* Kilit degiskeni (lock) neden yetmez: "sina" ile "kur" ayri adimlar oldugu icin
 * iki is parcacigi ayni anda lock == 0 gorup birlikte kritik bolgeye girebilir.
 * Kritik bolge problemi iskeleti: giris bolumu, kritik bolge, cikis, kalan bolum. */
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

#define TUR 200000

/* Paylasilan kilit. Okuma ve yazma ayri ayri atomik (tanimli davranis icin),
 * ama "oku, sonra 1 yaz" birlikte atomik DEGIL: slayttaki hatanin aynisi. */
atomic_int lock = 0;            /* paylasilan */

static atomic_int icerideki = 0;   /* kritik bolgedeki is parcacigi sayisi */
static atomic_int ihlal = 0;       /* ayni anda iki kisi iceride goruldu mu */
static long sayac = 0;             /* korunmasi gereken veri */

void gir(void)
{
    while (atomic_load_explicit(&lock, memory_order_relaxed) != 0)
        ;                       /* mesgul bekle */
    atomic_store_explicit(&lock, 1, memory_order_relaxed);
}

void cik(void)
{
    atomic_store_explicit(&lock, 0, memory_order_relaxed);
}

static void kritik_bolge(void)
{
    if (atomic_fetch_add(&icerideki, 1) != 0)
        atomic_fetch_add(&ihlal, 1);
    sayac++;                    /* bilerek korumasiz: yaris burada gorunur */
    atomic_fetch_sub(&icerideki, 1);
}

static void *surec(void *arg)
{
    (void)arg;
    for (int k = 0; k < TUR; k++) {      /* while (true) yerine sinirli tur */
        gir();                  /* giris bolumu (entry) */
        kritik_bolge();         /* kritik bolge */
        cik();                  /* cikis bolumu (exit) */
        /* kalan bolum (remainder) */
    }
    return NULL;
}

int main(void)
{
    pthread_t t[2];
    for (int i = 0; i < 2; i++)
        pthread_create(&t[i], NULL, surec, NULL);
    for (int i = 0; i < 2; i++)
        pthread_join(t[i], NULL);

    printf("beklenen sayac = %d\n", 2 * TUR);
    printf("bulunan sayac  = %ld\n", sayac);
    printf("karsilikli dislama ihlali: %d kez\n", atomic_load(&ihlal));
    return 0;
}

/* Beklenen cikti (yaris durumu: sayilar her calistirmada degisir; cok cekirdekli
 * makinede genellikle ihlal > 0 ve sayac < 400000 gorulur, sansla 0 da olabilir):
 * beklenen sayac = 400000
 * bulunan sayac  = 39xxxx
 * karsilikli dislama ihlali: N kez
 */
