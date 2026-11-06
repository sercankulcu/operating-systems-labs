/* Kesme gudumlu G/C ile yazici: sistem cagrisi ilk karakteri yazip sureci
 * bloke eder; her karakter basildiginda yazici kesme uretir ve kesme hizmet
 * yordami siradakini yazar. Kesme, yazici is parcacigindan cagrilan bir
 * fonksiyonla benzetilir. */
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>

#define READY 0

/* durum ve veri yazmaci ayni hucre: veri yazmak yaziciyi mesgul yapar */
static atomic_int yazmac = READY;
static atomic_int *printer_status_reg = &yazmac;
static atomic_int *printer_data_register = &yazmac;

static char buffer[64];       /* cekirdek tamponu */
static const char *p;         /* kesme yordaminin da gordugu durum */
static int i, count;
static int kesme_sayisi = 0;

static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t kesme_kilidi = PTHREAD_MUTEX_INITIALIZER; /* i, count */
static pthread_cond_t uyan = PTHREAD_COND_INITIALIZER;
static int bloke = 0;
static atomic_int kesmeler_acik = 0;
static atomic_int bitti = 0;
static pthread_t yazici;

static void yazici_kesmesi(void);

/* --- yazici (aygit): karakteri basar, sonra kesme uretir --- */
static void *yazici_calis(void *arg)
{
    (void)arg;
    while (!atomic_load(&bitti)) {
        int c = atomic_load(&yazmac);
        if (c == READY)
            continue;                         /* yazilacak karakter yok */
        struct timespec ts = { 0, 1000000 };  /* basmak 1 ms surer */
        nanosleep(&ts, NULL);
        putchar(c);
        fflush(stdout);
        atomic_store(&yazmac, READY);
        if (atomic_load(&kesmeler_acik))
            yazici_kesmesi();                 /* kesme hatti */
    }
    return NULL;
}

static void copy_from_user(char *hedef, const char *kaynak, int n)
{
    memcpy(hedef, kaynak, (size_t)n);
}

static void enable_interrupts(void)
{
    atomic_store(&kesmeler_acik, 1);
}

static void scheduler(void)
{
    /* surec bloke: CPU baska is yapabilir; burada uyandirilana dek bekler */
    pthread_mutex_lock(&m);
    while (bloke)
        pthread_cond_wait(&uyan, &m);
    pthread_mutex_unlock(&m);
}

static void unblock_user(void)
{
    pthread_mutex_lock(&m);
    bloke = 0;
    atomic_store(&bitti, 1);
    pthread_cond_signal(&uyan);
    pthread_mutex_unlock(&m);
}

static void acknowledge_interrupt(void)
{
    kesme_sayisi++;
}

static void return_from_interrupt(void)
{
    /* gercekte iret: kesilen surece donulur */
}

/* --- kesme hizmet yordami --- */
static void yazici_kesmesi(void)
{
    pthread_mutex_lock(&kesme_kilidi);
    if (count == 0) {
        unblock_user();
    } else {
        *printer_data_register = p[i];
        count = count - 1;
        i = i + 1;
    }
    acknowledge_interrupt();
    pthread_mutex_unlock(&kesme_kilidi);
    return_from_interrupt();
}

/* --- sistem cagrisi --- */
static void sistem_cagrisi(const char *kullanici, int n)
{
    p = kullanici;
    count = n;
    bloke = 1;
    pthread_create(&yazici, NULL, yazici_calis, NULL);

    copy_from_user(buffer, p, count);
    enable_interrupts();
    while (*printer_status_reg != READY) ;
    pthread_mutex_lock(&kesme_kilidi);     /* benzetim: kesmeyi kisa sure ertele */
    *printer_data_register = p[0];
    i = 1; count = count - 1;
    pthread_mutex_unlock(&kesme_kilidi);
    scheduler();      /* surec bloke */

    pthread_join(yazici, NULL);
    printf("\n%d kesme\n", kesme_sayisi);
}

int main(void)
{
    sistem_cagrisi("ABC", 3);
    return 0;
}

/* Beklenen cikti:
 * ABC
 * 3 kesme
 */
