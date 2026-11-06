/* Kilitlenme disi hata 2: sira ihlali. T1 "isci = thread_olustur(govde)" atamasinin
 * yeni is parcacigi (T2) govde()'ye girmeden once olacagini varsayar; bu sira
 * hicbir seyle zorlanmaz. Care: hazir bayragi + kosul degiskeni. */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct isci_t {
    pthread_t tid;
    int durum;
};

static struct isci_t *isci;                 /* T1 atar, T2 okur */
static int duzeltilmis;                     /* 0: hatali, 1: hazir bayragiyla */
static int hazir;
static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t c = PTHREAD_COND_INITIALIZER;

/* pthread_create'i saran yardimci; donmesi 50 ms gecikir (yavas donus) */
static struct isci_t *thread_olustur(void *(*f)(void *))
{
    struct isci_t *y = malloc(sizeof *y);
    y->durum = 7;
    pthread_create(&y->tid, NULL, f, NULL);
    usleep(50000);
    return y;
}

/* T2: govde(), yeni is parcacigi */
static void *govde(void *arg)
{
    (void)arg;
    int d;

    if (duzeltilmis) {
        pthread_mutex_lock(&m);
        while (!hazir)
            pthread_cond_wait(&c, &m);
        pthread_mutex_unlock(&m);
    }
    pthread_mutex_lock(&m);                 /* okumayi gorunur kilmak icin */
    struct isci_t *i = isci;
    pthread_mutex_unlock(&m);
    if (i == NULL) {                        /* sunumdaki isci->durum burada cokerdi */
        printf("T2: isci henuz atanmadi (NULL)! sira ihlali\n");
        return NULL;
    }
    d = i->durum;  /* isci atandi mi? */
    printf("T2: d = isci->durum = %d\n", d);
    return NULL;
}

/* T1: baslat() */
static void baslat(void)
{
    struct isci_t *y = thread_olustur(govde);

    pthread_mutex_lock(&m);
    isci = y;
    hazir = 1;
    pthread_cond_signal(&c);
    pthread_mutex_unlock(&m);
}

static void temizle(void)
{
    pthread_join(isci->tid, NULL);
    free(isci);
    isci = NULL;
    hazir = 0;
}

int main(void)
{
    printf("Hatali surum:\n");
    duzeltilmis = 0;
    baslat();
    temizle();

    printf("hazir bayragi + kosul degiskeni ile:\n");
    duzeltilmis = 1;
    baslat();
    temizle();
    return 0;
}

/* Beklenen cikti (zamanlamaya bagli; 50 ms gecikme sonucu neredeyse kesin kilar):
 * Hatali surum:
 * T2: isci henuz atanmadi (NULL)! sira ihlali
 * hazir bayragi + kosul degiskeni ile:
 * T2: d = isci->durum = 7
 */
