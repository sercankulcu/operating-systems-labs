/* Kati sirayla degisim (strict alternation): turn degiskeni P0 ve P1'i sirayla
 * kritik bolgeye sokar. Karsilikli dislama saglanir ama ilerleme saglanmaz:
 * biri kritik disinda yavaslarsa digeri de beklemek zorunda kalir. */
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>

#define TUR 4

atomic_int turn = 0;

static char gunluk[64];          /* kritik bolgeye giris sirasi */
static int gunluk_n = 0;

static void kritik_bolge(int i)
{
    gunluk[gunluk_n++] = (char)('0' + i);   /* turn sayesinde tek kisi yazar */
}

static void kritik_disi(int i)
{
    /* P1 kritik disinda yavas: P0 bu surede ikinci kez giremez */
    if (i == 1) {
        struct timespec ts = { 0, 2000000 };  /* 2 ms */
        nanosleep(&ts, NULL);
    }
}

static void *P0(void *arg)
{
    (void)arg;
    for (int k = 0; k < TUR; k++) {          /* while (true) yerine TUR tur */
        while (atomic_load(&turn) != 0)
            ;                                 /* bekle */
        kritik_bolge(0);
        atomic_store(&turn, 1);
        kritik_disi(0);
    }
    return NULL;
}

static void *P1(void *arg)
{
    (void)arg;
    for (int k = 0; k < TUR; k++) {
        while (atomic_load(&turn) != 1)
            ;                                 /* bekle */
        kritik_bolge(1);
        atomic_store(&turn, 0);
        kritik_disi(1);
    }
    return NULL;
}

int main(void)
{
    pthread_t t0, t1;
    pthread_create(&t0, NULL, P0, NULL);
    pthread_create(&t1, NULL, P1, NULL);
    pthread_join(t0, NULL);
    pthread_join(t1, NULL);

    gunluk[gunluk_n] = '\0';
    printf("kritik bolge sirasi: %s\n", gunluk);
    printf("turn = %d\n", atomic_load(&turn));
    return 0;
}

/* Beklenen cikti (her calistirmada ayni: sira turn ile zorlanir):
 * kritik bolge sirasi: 01010101
 * turn = 0
 */
