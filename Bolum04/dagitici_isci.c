/* Cok is parcacikli web sunucusu: dagitici is parcacigi istekleri alip bos
 * isciye verir; isci sayfayi onbellekte arar, yoksa diskten okur (bloke olur).
 * Ag ve disk taklit edilir; sonsuz donguler istek bitince sona erer. */
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define TRUE 1
#define ISCI_SAYISI 2

struct istek { char sayfa[32]; int istemci; };   /* buf */
struct sayfa { char ad[32]; int bulundu; int istemci; };   /* page */

/* ---- ag: gelecek istekler ---- */
static const char *gelen_sayfalar[] = {"b.html", "a.html"};
static int gelen_sira = 0;

/* ---- onbellek: basta yalnizca a.html var ---- */
static char onbellek[4][32] = {"a.html"};
static int onbellek_adet = 1;
static pthread_mutex_t onbellek_kilit = PTHREAD_MUTEX_INITIALIZER;

/* ---- dagiticidan isciye is devri ---- */
static struct istek is_yuvasi;
static int is_var = 0;
static pthread_mutex_t is_kilit = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t is_geldi = PTHREAD_COND_INITIALIZER;
static pthread_cond_t yuva_bos = PTHREAD_COND_INITIALIZER;

/* ---- yanitlar: tamamlanma sirasi ---- */
static char yanitlar[8][64];
static int yanit_adet = 0;
static pthread_mutex_t yanit_kilit = PTHREAD_MUTEX_INITIALIZER;

static void uyu_ms(long ms)
{
    struct timespec t = { ms / 1000, (ms % 1000) * 1000000L };
    nanosleep(&t, NULL);
}

/* Agdan sonraki istegi okur; istek kalmadiysa 0 dondurur. */
static int get_next_request(struct istek *buf)
{
    if (gelen_sira >= 2)
        return 0;
    uyu_ms(20);                                  /* istekler arasi */
    strcpy(buf->sayfa, gelen_sayfalar[gelen_sira]);
    buf->istemci = gelen_sira + 1;
    gelen_sira++;
    return 1;
}

static void handoff_work(struct istek *buf)
{
    pthread_mutex_lock(&is_kilit);
    while (is_var)
        pthread_cond_wait(&yuva_bos, &is_kilit);
    is_yuvasi = *buf;
    is_var = 1;
    pthread_cond_signal(&is_geldi);              /* bos bir isciyi uyandir */
    pthread_mutex_unlock(&is_kilit);
}

static void wait_for_work(struct istek *buf)
{
    pthread_mutex_lock(&is_kilit);
    while (!is_var)
        pthread_cond_wait(&is_geldi, &is_kilit);
    *buf = is_yuvasi;
    is_var = 0;
    pthread_cond_signal(&yuva_bos);
    pthread_mutex_unlock(&is_kilit);
}

static void look_for_page_in_cache(struct istek *buf, struct sayfa *page)
{
    strcpy(page->ad, buf->sayfa);
    page->istemci = buf->istemci;
    page->bulundu = 0;
    pthread_mutex_lock(&onbellek_kilit);
    for (int i = 0; i < onbellek_adet; i++)
        if (strcmp(onbellek[i], buf->sayfa) == 0)
            page->bulundu = 1;
    pthread_mutex_unlock(&onbellek_kilit);
}

static int page_not_in_cache(struct sayfa *page)
{
    return !page->bulundu;
}

static void read_page_from_disk(struct istek *buf, struct sayfa *page)
{
    uyu_ms(200);                                 /* disk: yalnizca bu isci bloke */
    pthread_mutex_lock(&onbellek_kilit);
    if (onbellek_adet < 4)
        strcpy(onbellek[onbellek_adet++], buf->sayfa);
    pthread_mutex_unlock(&onbellek_kilit);
    page->bulundu = 1;
}

static void return_page(struct sayfa *page)
{
    pthread_mutex_lock(&yanit_kilit);
    snprintf(yanitlar[yanit_adet++], sizeof yanitlar[0], "%.31s -> istemci %d",
             page->ad, page->istemci);
    pthread_mutex_unlock(&yanit_kilit);
}

/* dagitici is parcacigi */
static void *dagitici(void *arg)
{
    (void)arg;
    struct istek buf = {"", 0};
    while (TRUE) {
        if (!get_next_request(&buf))
            break;                               /* benzetim: istek bitti */
        handoff_work(&buf);
    }
    for (int i = 0; i < ISCI_SAYISI; i++) {      /* iscilere "dur" istegi */
        buf.sayfa[0] = '\0';
        handoff_work(&buf);
    }
    return NULL;
}

/* isci is parcacigi */
static void *isci(void *arg)
{
    (void)arg;
    struct istek buf = {"", 0};
    struct sayfa page = {"", 0, 0};
    while (TRUE) {
        wait_for_work(&buf);
        if (buf.sayfa[0] == '\0')
            break;
        look_for_page_in_cache(&buf, &page);
        if (page_not_in_cache(&page))
            read_page_from_disk(&buf, &page);
        return_page(&page);
    }
    return NULL;
}

int main(void)
{
    pthread_t d, w[ISCI_SAYISI];
    for (int i = 0; i < ISCI_SAYISI; i++)
        pthread_create(&w[i], NULL, isci, NULL);
    pthread_create(&d, NULL, dagitici, NULL);

    pthread_join(d, NULL);
    for (int i = 0; i < ISCI_SAYISI; i++)
        pthread_join(w[i], NULL);

    for (int i = 0; i < yanit_adet; i++)
        printf("%s\n", yanitlar[i]);
    return 0;
}

/* Beklenen cikti (istek 2 onbellekten, istek 1 diskten yanitlanir; siralama
 * disk gecikmesinden (200 ms) gelir, asiri yuklu bir makinede degisebilir):
 * a.html -> istemci 2
 * b.html -> istemci 1
 */
