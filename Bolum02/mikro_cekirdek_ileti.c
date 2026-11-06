/* Mikro cekirdekte send/receive: istemci, dosya sistemi sunucusu (FS) ve
 * disk surucusu (DISK) ayri "surecler"dir (burada iplikler) ve yalnizca
 * cekirdegin tasidigi iletilerle konusur. Cekirdek posta kutulari taklit edilir. */
#include <stdio.h>
#include <string.h>
#include <pthread.h>

/* libc'deki send() ile karismasin diye adlar makroyla degistirilir. */
#define send(hedef, ileti)    cekirdek_send((hedef), (ileti), sizeof *(ileti))
#define receive(kaynak, ileti) cekirdek_receive((kaynak), (ileti), sizeof *(ileti))

enum { ISTEMCI, FS, DISK, SUREC_SAYISI, ANY = -1 };
enum { OKU = 1, DUR = 2 };
static const char *ad[] = {"istemci", "FS", "DISK"};

struct blok_verisi { char icerik[24]; };
struct mesaj {
    int tur, dosya, ofset;
    int kaynak;                     /* cekirdek doldurur */
    struct blok_verisi sonuc;
};

/* ---- cekirdek: her surec icin bir posta kutusu ---- */
#define KUTU 8
struct posta { int kaynak; size_t boy; unsigned char veri[64]; };
static struct posta kutu[SUREC_SAYISI][KUTU];
static int kutu_adet[SUREC_SAYISI];
static pthread_mutex_t kilit = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t degisti = PTHREAD_COND_INITIALIZER;
static _Thread_local int ben;      /* cagiran surecin kimligi */

static void cekirdek_send(int hedef, const void *ileti, size_t boy)
{
    pthread_mutex_lock(&kilit);
    struct posta *p = &kutu[hedef][kutu_adet[hedef]++];
    p->kaynak = ben;
    p->boy = boy;
    memcpy(p->veri, ileti, boy);
    if (boy == sizeof(struct mesaj))           /* m.kaynak'i cekirdek yazar */
        ((struct mesaj *)(void *)p->veri)->kaynak = ben;
    printf("  send: %s -> %s\n", ad[ben], ad[hedef]);
    pthread_cond_broadcast(&degisti);
    pthread_mutex_unlock(&kilit);
}

/* kaynak == ANY ise herhangi bir gondericiden, yoksa yalniz ondan alir. */
static void cekirdek_receive(int kaynak, void *ileti, size_t boy)
{
    pthread_mutex_lock(&kilit);
    for (;;) {
        for (int i = 0; i < kutu_adet[ben]; i++) {
            struct posta *p = &kutu[ben][i];
            if (kaynak == ANY || p->kaynak == kaynak) {
                memcpy(ileti, p->veri, boy < p->boy ? boy : p->boy);
                for (int j = i + 1; j < kutu_adet[ben]; j++)
                    kutu[ben][j - 1] = kutu[ben][j];
                kutu_adet[ben]--;
                pthread_mutex_unlock(&kilit);
                return;
            }
        }
        pthread_cond_wait(&degisti, &kilit);   /* bloke ol */
    }
}

/* ---- dosya sistemi sunucusu ---- */
static int blok_bul(int dosya, int ofset)
{
    static const int ilk_blok[8] = {10, 20, 30, 40, 50, 60, 70, 120};
    return ilk_blok[dosya] + ofset / 4096;
}

static void *fs_sunucusu(void *arg)
{
    (void)arg;
    ben = FS;
    struct mesaj m;
    int blok;
    struct blok_verisi veri;
    while (1) {
        receive(ANY, &m);
        if (m.tur == DUR) {                    /* benzetimi bitir */
            blok = -1;
            send(DISK, &blok);
            break;
        }
        blok = blok_bul(m.dosya, m.ofset);
        printf("  FS: dosya %d, ofset %d -> blok %d\n", m.dosya, m.ofset, blok);
        send(DISK, &blok);
        receive(DISK, &veri);
        m.sonuc = veri;
        send(m.kaynak, &m);
    }
    return NULL;
}

/* ---- disk surucusu ---- */
static void *disk_surucusu(void *arg)
{
    (void)arg;
    ben = DISK;
    int blok;
    struct blok_verisi veri;
    while (1) {
        receive(FS, &blok);
        if (blok < 0)
            break;
        snprintf(veri.icerik, sizeof veri.icerik, "blok-%d-verisi", blok);
        printf("  DISK: blok %d okundu\n", blok);
        send(FS, &veri);
    }
    return NULL;
}

int main(void)
{
    pthread_t fs_t, disk_t;
    pthread_create(&fs_t, NULL, fs_sunucusu, NULL);
    pthread_create(&disk_t, NULL, disk_surucusu, NULL);

    /* istemci */
    ben = ISTEMCI;
    struct mesaj m;
    memset(&m, 0, sizeof m);
    m.tur = OKU; m.dosya = 7; m.ofset = 0;
    printf("istemci: OKU dosya=7 ofset=0\n");
    send(FS, &m);
    receive(FS, &m);       /* yaniti bekle */
    printf("istemci: yanit \"%s\"\n", m.sonuc.icerik);

    m.tur = DUR;
    send(FS, &m);
    pthread_join(fs_t, NULL);
    pthread_join(disk_t, NULL);
    return 0;
}

/* Beklenen cikti (her adim bir oncekinin iletisini bekledigi icin sira sabittir):
 * istemci: OKU dosya=7 ofset=0
 *   send: istemci -> FS
 *   FS: dosya 7, ofset 0 -> blok 120
 *   send: FS -> DISK
 *   DISK: blok 120 okundu
 *   send: DISK -> FS
 *   send: FS -> istemci
 * istemci: yanit "blok-120-verisi"
 *   send: istemci -> FS
 *   send: FS -> DISK
 */
