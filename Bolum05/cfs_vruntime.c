/* CFS vruntime: her tikta sanal calisma suresi en kucuk (agacin en solundaki)
 * gorev calisir; vruntime'i DELTA * 1024 / agirlik kadar artar. Kirmizi-siyah
 * agac yerine vruntime'a gore sirali bir bagli liste kullanilir.
 * A: nice 0 (agirlik 1024), B: nice 5 (agirlik 335). */
#include <stdio.h>

#define DELTA 3000  /* 3 ms, mikrosaniye olarak */
/* nice 0 -> 1024, nice 5 -> 335 */

typedef struct Gorev {
    char ad;
    int agirlik;
    long vruntime;
    int tik_sayisi;
    struct Gorev *sonraki;
} Gorev;

static Gorev *agac = NULL;      /* en soldaki basta */

/* Esit vruntime'lar arasina SONA ekler (cekirdekteki agac da boyle yapar). */
static void agaca_ekle(Gorev *g)
{
    Gorev **p = &agac;
    while (*p && (*p)->vruntime <= g->vruntime)
        p = &(*p)->sonraki;
    g->sonraki = *p;
    *p = g;
}

/* En soldaki gorevi agactan cikarir (calisan gorev agacta durmaz). */
static Gorev *en_soldaki(void)
{
    Gorev *g = agac;
    agac = g->sonraki;
    g->sonraki = NULL;
    return g;
}

static void calistir(Gorev *g, int sure)
{
    (void)sure;                 /* gercekte: sure kadar CPU */
    g->tik_sayisi++;
}

void tik(void)
{
    Gorev *g = en_soldaki(); /* en solda */
    calistir(g, DELTA);
    g->vruntime += DELTA * 1024 / g->agirlik;
    agaca_ekle(g);      /* yeniden sirala */
}

int main(void)
{
    Gorev A = {'A', 1024, 0, 0, NULL};
    Gorev B = {'B', 335, 0, 0, NULL};
    agaca_ekle(&A);
    agaca_ekle(&B);

    for (int i = 1; i <= 9; i++) {
        char calisan = agac->ad;
        tik();
        printf("tik %d: %c  A.vruntime=%5ld  B.vruntime=%5ld\n",
               i, calisan, A.vruntime, B.vruntime);
    }
    printf("A=%d tik, B=%d tik\n", A.tik_sayisi, B.tik_sayisi);
    return 0;
}

/* Beklenen cikti:
 * tik 1: A  A.vruntime= 3000  B.vruntime=    0
 * tik 2: B  A.vruntime= 3000  B.vruntime= 9170
 * tik 3: A  A.vruntime= 6000  B.vruntime= 9170
 * tik 4: A  A.vruntime= 9000  B.vruntime= 9170
 * tik 5: A  A.vruntime=12000  B.vruntime= 9170
 * tik 6: B  A.vruntime=12000  B.vruntime=18340
 * tik 7: A  A.vruntime=15000  B.vruntime=18340
 * tik 8: A  A.vruntime=18000  B.vruntime=18340
 * tik 9: A  A.vruntime=21000  B.vruntime=18340
 * A=7 tik, B=2 tik
 */
