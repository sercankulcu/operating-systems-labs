/* Acilir (pop-up) is parcaciklari: her ileti gelince onu isleyecek yeni bir
 * is parcacigi olusturulur ve ayrilir (detach); join edilmez. Program
 * bitmeden once tum ayrik is parcaciklarinin bitmesi bir sayacla beklenir. */
#include <stdio.h>
#include <pthread.h>

#define ILETI_SAYISI 3

static const char *iletiler[ILETI_SAYISI] = {"GET /a", "GET /b", "GET /c"};
static char sonuc[ILETI_SAYISI][64];
static int biten = 0;
static pthread_mutex_t kilit = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t bitti = PTHREAD_COND_INITIALIZER;

static void *isle(void *arg)
{
    const char **msg = arg;
    int no = (int)(msg - iletiler);
    snprintf(sonuc[no], sizeof sonuc[no], "ileti %d (%s) islendi", no, *msg);
    pthread_mutex_lock(&kilit);
    biten++;
    pthread_cond_signal(&bitti);
    pthread_mutex_unlock(&kilit);
    return NULL;
}

int main(void)
{
    for (int i = 0; i < ILETI_SAYISI; i++) {
        const char **msg = &iletiler[i];
        /* mesaj gelince */
        pthread_t t;
        pthread_create(&t, NULL, isle, msg);
        pthread_detach(t); /* join yok */
    }

    /* Ayrik is parcaciklari join edilemez: bitis sayaci beklenir. */
    pthread_mutex_lock(&kilit);
    while (biten < ILETI_SAYISI)
        pthread_cond_wait(&bitti, &kilit);
    pthread_mutex_unlock(&kilit);

    for (int i = 0; i < ILETI_SAYISI; i++)   /* sirali yazdir */
        printf("%s\n", sonuc[i]);
    return 0;
}

/* Beklenen cikti (is parcaciklari herhangi bir sirada bitebilir; yazdirma
 * sirasi sabittir):
 * ileti 0 (GET /a) islendi
 * ileti 1 (GET /b) islendi
 * ileti 2 (GET /c) islendi
 */
