/* Kilitlenme disi hata 1: atomiklik ihlali. T1 "ad NULL degil mi?" diye sinar,
 * kullanmadan once T2 araya girip ad'i NULL yapar. Care: sinama + kullanim ve
 * T2'nin yazmasi ayni mutex altinda. Pencere usleep ile buyutulmustur. */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

struct oturum_t {
    const char *ad;
    pthread_mutex_t kilit;
};

static struct oturum_t oturum_nesnesi = { "ayse", PTHREAD_MUTEX_INITIALIZER };
static struct oturum_t *oturum = &oturum_nesnesi;

/* ---- Hatali surum ---- */
static void *T1_hatali(void *arg)
{
    (void)arg;
    if (oturum->ad != NULL) {
        usleep(100000);                         /* araya girme penceresi */
        const char *ad = oturum->ad;
        if (ad != NULL)
            puts(ad);
        else    /* sunumdaki puts(oturum->ad) burada puts(NULL) olur: cokme */
            puts("T1: sinamada NULL degildi, simdi NULL! (atomiklik ihlali)");
    }
    return NULL;
}

static void *T2_hatali(void *arg)
{
    (void)arg;
    usleep(20000);                              /* T1 sinadiktan sonra */
    oturum->ad = NULL;
    return NULL;
}

/* ---- Duzeltilmis surum: ayni mutex ---- */
static void *T1(void *arg)
{
    (void)arg;
    pthread_mutex_lock(&oturum->kilit);
    if (oturum->ad != NULL) {
        usleep(100000);                         /* pencere ayni, ama kilit altinda */
        puts(oturum->ad);  /* artik NULL olamaz */
    }
    pthread_mutex_unlock(&oturum->kilit);
    return NULL;
}

static void *T2(void *arg)
{
    (void)arg;
    usleep(20000);
    pthread_mutex_lock(&oturum->kilit);
    oturum->ad = NULL;
    pthread_mutex_unlock(&oturum->kilit);
    return NULL;
}

static void calistir(void *(*f1)(void *), void *(*f2)(void *))
{
    pthread_t a, b;

    oturum->ad = "ayse";
    pthread_create(&a, NULL, f1, NULL);
    pthread_create(&b, NULL, f2, NULL);
    pthread_join(a, NULL);
    pthread_join(b, NULL);
    printf("sonra: oturum->ad = %s\n\n", oturum->ad ? oturum->ad : "NULL");
}

int main(void)
{
    printf("Hatali surum:\n");
    calistir(T1_hatali, T2_hatali);
    printf("Mutex ile:\n");
    calistir(T1, T2);
    return 0;
}

/* Beklenen cikti (zamanlamaya bagli; 100 ms pencere sonucu neredeyse kesin kilar):
 * Hatali surum:
 * T1: sinamada NULL degildi, simdi NULL! (atomiklik ihlali)
 * sonra: oturum->ad = NULL
 *
 * Mutex ile:
 * ayse
 * sonra: oturum->ad = NULL
 */
