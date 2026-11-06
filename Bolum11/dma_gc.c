/* DMA ile G/C: sistem cagrisi DMA denetleyicisini programlayip sureci bloke
 * eder; DMA tamponun tamamini CPU'suz yaziciya aktarir ve sonunda tek bir
 * kesme uretir. DMA denetleyicisi ayri bir is parcacigi olarak benzetilir. */
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

static char buffer[64];          /* cekirdek tamponu: DMA buradan okur */
static int dma_sayac;            /* DMA'nin aktaracagi bayt sayisi */
static int kesme_sayisi = 0;

static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t uyan = PTHREAD_COND_INITIALIZER;
static int bloke = 0;
static pthread_t dma;

static void dma_kesmesi(void);

/* --- DMA denetleyicisi: bayt bayt yaziciya aktarir, CPU'ya dokunmaz --- */
static void *dma_calis(void *arg)
{
    (void)arg;
    for (int k = 0; k < dma_sayac; k++) {
        struct timespec ts = { 0, 1000000 };   /* her karakter 1 ms */
        nanosleep(&ts, NULL);
        putchar(buffer[k]);                     /* yazici kapisina yaz */
    }
    putchar('\n');
    fflush(stdout);
    dma_kesmesi();                              /* is bitti: tek kesme */
    return NULL;
}

static void copy_from_user(char *hedef, const char *kaynak, int n)
{
    memcpy(hedef, kaynak, (size_t)n);
}

static void set_up_DMA_controller(void)
{
    /* tampon adresi, sayac ve hedef kapi programlanir, aktarim baslar */
    pthread_create(&dma, NULL, dma_calis, NULL);
}

static void scheduler(void)
{
    pthread_mutex_lock(&m);
    while (bloke)
        pthread_cond_wait(&uyan, &m);     /* surec bloke */
    pthread_mutex_unlock(&m);
}

static void acknowledge_interrupt(void)
{
    kesme_sayisi++;
}

static void unblock_user(void)
{
    pthread_mutex_lock(&m);
    bloke = 0;
    pthread_cond_signal(&uyan);
    pthread_mutex_unlock(&m);
}

static void return_from_interrupt(void)
{
    /* gercekte iret */
}

/* --- kesme hizmet yordami --- */
static void dma_kesmesi(void)
{
    acknowledge_interrupt();
    unblock_user();
    return_from_interrupt();
}

/* --- sistem cagrisi --- */
static void sistem_cagrisi(const char *p, int count)
{
    bloke = 1;
    dma_sayac = count;
    copy_from_user(buffer, p, count);
    set_up_DMA_controller();
    scheduler();     /* surec bloke */

    pthread_join(dma, NULL);
    printf("%d karakter, %d kesme\n", count, kesme_sayisi);
}

int main(void)
{
    sistem_cagrisi("ABC", 3);
    return 0;
}

/* Beklenen cikti:
 * ABC
 * 3 karakter, 1 kesme
 */
