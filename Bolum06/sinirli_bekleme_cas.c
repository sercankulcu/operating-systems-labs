/* Sinirli bekleme ile CAS: cikan surec kilidi birakmak yerine sirayi halkada
 * bekleyen ilk surece (i+1, i+2, ...) devreder; boylece hicbir surec n-1'den fazla
 * tur beklemez. Once slayttaki iz (n = 4; P2 icerde, P0 ve P3 bekliyor) gercek is
 * parcaciklariyla, ana is parcacigi sirayi yoneterek oynatilir; sonra serbest kosu. */
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdatomic.h>
#include <sched.h>

#define n 4
#define TUR 10000

atomic_bool waiting[n];
atomic_int  lock = 0;

int compare_and_swap(atomic_int *v, int bekl, int yeni)
{
    int eski = bekl;
    (void)atomic_compare_exchange_strong(v, &eski, yeni);
    return eski;
}

/* iz icin: ana is parcacigi kritik bolgeden cikis iznini verir */
static bool adimli = false;
static atomic_bool devam[n];
static atomic_int  kb_sahibi = -1;
static char kb_sirasi[16];
static int  kb_n = 0;

static long sayac = 0;
static atomic_int icerideki = 0, ihlal = 0;

static void yavas_don(unsigned *donme)
{
    if (++*donme % 1024 == 0)
        sched_yield();
}

static void kritik_bolge(int i)
{
    if (atomic_fetch_add(&icerideki, 1) != 0)
        atomic_fetch_add(&ihlal, 1);
    sayac++;
    if (adimli) {
        kb_sirasi[kb_n++] = (char)('0' + i);
        atomic_store(&kb_sahibi, i);
        unsigned d = 0;
        while (!atomic_load(&devam[i]))
            yavas_don(&d);
    }
    atomic_fetch_sub(&icerideki, 1);
}

static void surec(int i, int tur)
{
    int key, j;
    for (int k = 0; k < tur; k++) {          /* while (true) yerine sinirli tur */
        unsigned d = 0;
        atomic_store(&waiting[i], true);
        key = 1;
        while (atomic_load(&waiting[i]) && key == 1) {
            key = compare_and_swap(&lock, 0, 1);
            if (key == 1)
                yavas_don(&d);
        }
        atomic_store(&waiting[i], false);
        kritik_bolge(i);                     /* kritik bolge */
        j = (i + 1) % n;
        while ((j != i) && !atomic_load(&waiting[j]))
            j = (j + 1) % n;
        if (j == i)
            atomic_store(&lock, 0);          /* bekleyen yok */
        else
            atomic_store(&waiting[j], false); /* sirayi devret */
        /* kalan bolum */
    }
}

static int tur_sayisi;

static void *P(void *arg)
{
    surec(*(int *)arg, tur_sayisi);
    return NULL;
}

static void bekle_kb(int i)
{
    unsigned d = 0;
    while (atomic_load(&kb_sahibi) != i)
        yavas_don(&d);
}

static void bekle_waiting(int i)
{
    unsigned d = 0;
    while (!atomic_load(&waiting[i]))
        yavas_don(&d);
}

int main(void)
{
    pthread_t t[n];
    int id[n] = { 0, 1, 2, 3 };

    /* 1) Slayttaki iz: P2 girer; P0 ve P3 bekler; P2 -> P3 -> P0; lock = 0 */
    adimli = true;
    tur_sayisi = 1;
    pthread_create(&t[2], NULL, P, &id[2]);
    bekle_kb(2);                           /* P2 kritik bolgede */
    pthread_create(&t[0], NULL, P, &id[0]);
    bekle_waiting(0);                      /* waiting = [T F F F] */
    pthread_create(&t[3], NULL, P, &id[3]);
    bekle_waiting(3);                      /* waiting = [T F F T] */
    atomic_store(&devam[2], true);         /* P2 cikar: j = 3 -> waiting[3] = false */
    bekle_kb(3);
    atomic_store(&devam[3], true);         /* P3 cikar: j = 0 -> waiting[0] = false */
    bekle_kb(0);
    atomic_store(&devam[0], true);         /* P0 cikar: bekleyen yok -> lock = 0 */
    pthread_join(t[2], NULL);
    pthread_join(t[3], NULL);
    pthread_join(t[0], NULL);
    kb_sirasi[kb_n] = '\0';
    printf("kritik bolge sirasi:");
    for (int k = 0; k < kb_n; k++)
        printf(" P%c", kb_sirasi[k]);
    printf("\nlock = %d\n", atomic_load(&lock));

    /* 2) Serbest kosu: 4 surec x TUR */
    adimli = false;
    tur_sayisi = TUR;
    sayac = 0;
    for (int i = 0; i < n; i++)
        pthread_create(&t[i], NULL, P, &id[i]);
    for (int i = 0; i < n; i++)
        pthread_join(t[i], NULL);
    printf("4 surec x %d: sayac = %ld, ihlal = %d, lock = %d\n",
           TUR, sayac, atomic_load(&ihlal), atomic_load(&lock));
    return 0;
}

/* Beklenen cikti (her calistirmada ayni):
 * kritik bolge sirasi: P2 P3 P0
 * lock = 0
 * 4 surec x 10000: sayac = 40000, ihlal = 0, lock = 0
 */
