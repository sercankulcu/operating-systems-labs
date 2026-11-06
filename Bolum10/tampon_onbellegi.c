/* Tampon onbellegi (buffer cache): bread isabette tamponu LRU listesinin sonuna
 * tasir; iskada en eski tamponu yeniden kullanir, kirliyse once diske yazar.
 * 3 tampon: bloklar 5, 9 (kirli), 2. Istekler: 9, 7, 2, 4. */
#include <stdio.h>

#define TAMPON_SAYISI 3

typedef struct Tampon {
    int blok;
    int kirli;
    struct Tampon *onceki, *sonraki;    /* LRU listesi: bas en eski */
} Tampon;

static Tampon havuz[TAMPON_SAYISI];
static Tampon *bas, *kuyruk;
static int okuma, yazma, son_yazilan = -1;

/* Kucuk ornek: hash tablosu yerine havuzda dogrusal arama */
static Tampon *hash_ara(int blok)
{
    for (int i = 0; i < TAMPON_SAYISI; i++)
        if (havuz[i].blok == blok)
            return &havuz[i];
    return NULL;
}

static void listeden_cikar(Tampon *t)
{
    if (t->onceki) t->onceki->sonraki = t->sonraki; else bas = t->sonraki;
    if (t->sonraki) t->sonraki->onceki = t->onceki; else kuyruk = t->onceki;
    t->onceki = t->sonraki = NULL;
}

static void lru_sona(Tampon *t)             /* en yeni yap */
{
    if (t->onceki || t->sonraki || bas == t)
        listeden_cikar(t);
    t->onceki = kuyruk;
    t->sonraki = NULL;
    if (kuyruk) kuyruk->sonraki = t; else bas = t;
    kuyruk = t;
}

static Tampon *lru_bas(void)                /* en eski */
{
    return bas;
}

static void diske_yaz(Tampon *t)
{
    yazma++;
    son_yazilan = t->blok;
    t->kirli = 0;
}

static void diskten_oku(Tampon *t)
{
    (void)t;
    okuma++;
}

Tampon *bread(int blok)
{
    Tampon *t = hash_ara(blok);
    if (t != NULL) {         /* isabet */
        lru_sona(t);
        return t;
    }
    t = lru_bas();           /* en eski */
    if (t->kirli)
        diske_yaz(t);        /* geri yaz */
    t->blok = blok; t->kirli = 0;
    diskten_oku(t);          /* iska: G/C */
    lru_sona(t);
    return t;
}

static void liste_yaz(void)
{
    printf("   LRU (eski -> yeni):");
    for (Tampon *t = bas; t != NULL; t = t->sonraki)
        printf(" %d%s", t->blok, t->kirli ? "*" : "");
    printf("\n");
}

int main(void)
{
    const int blok[3] = {5, 9, 2}, kirli[3] = {0, 1, 0};
    const int istek[4] = {9, 7, 2, 4};

    for (int i = 0; i < TAMPON_SAYISI; i++) {
        havuz[i].blok = blok[i];
        havuz[i].kirli = kirli[i];
        lru_sona(&havuz[i]);
    }
    liste_yaz();
    for (int i = 0; i < 4; i++) {
        int o = okuma, y = yazma;
        bread(istek[i]);
        if (okuma == o)
            printf("%d: isabet\n", istek[i]);
        else if (yazma > y)
            printf("%d: iska (%d geri yazildi)\n", istek[i], son_yazilan);
        else
            printf("%d: iska\n", istek[i]);
        liste_yaz();
    }
    printf("toplam: %d okuma, %d yazma\n", okuma, yazma);
    return 0;
}

/* Beklenen cikti (* kirli):
 *    LRU (eski -> yeni): 5 9* 2
 * 9: isabet
 *    LRU (eski -> yeni): 5 2 9*
 * 7: iska
 *    LRU (eski -> yeni): 2 9* 7
 * 2: isabet
 *    LRU (eski -> yeni): 9* 7 2
 * 4: iska (9 geri yazildi)
 *    LRU (eski -> yeni): 7 2 4
 * toplam: 2 okuma, 1 yazma
 */
