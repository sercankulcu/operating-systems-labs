/* Buddy (es) ayirici: 256 KB'lik tek blokla baslanir (mertebe 8, birim KB).
 * ayir(k) 2^k KB'lik blok verir, gerekirse buyuk blogu ikiye boler; birak()
 * kardesi (adres XOR boyut) bossa birlestirir. A = 21 KB -> 32 KB, B = 64 KB. */
#include <stdio.h>
#include <stdlib.h>

#define MAX 8                   /* 2^8 = 256 KB */

struct dugum {
    int adres;
    struct dugum *sonraki;
};

static struct dugum *bos[MAX + 1];  /* bos[k]: 2^k KB'lik bos bloklar */

static void ekle(int k, int a)
{
    struct dugum *d = malloc(sizeof *d);
    d->adres = a;
    d->sonraki = bos[k];
    bos[k] = d;
}

static int cek(int k)                /* bos[k]'nin basindaki blogu al */
{
    struct dugum *d = bos[k];
    int a = d->adres;
    bos[k] = d->sonraki;
    free(d);
    return a;
}

static int sil(int k, int a)         /* a, bos[k]'de ise cikar: 1 */
{
    for (struct dugum **p = &bos[k]; *p != NULL; p = &(*p)->sonraki) {
        if ((*p)->adres == a) {
            struct dugum *d = *p;
            *p = d->sonraki;
            free(d);
            return 1;
        }
    }
    return 0;
}

int ayir(int k)            /* 2^k KB; yeterli bos blok oldugu varsayilir */
{
    int j = k;
    while (bos[j] == NULL) j++;
    int a = cek(j);
    while (j > k) {   /* bol, sag yari bos */
        j--; ekle(j, a + (1 << j));
    }
    return a;
}

void birak(int a, int k)
{
    int b = a ^ (1 << k);    /* kardes */
    while (k < MAX && sil(k, b)) {
        a = a < b ? a : b;     /* birles */
        k++; b = a ^ (1 << k);
    }
    ekle(k, a);
}

static void listeleri_yaz(const char *baslik)
{
    printf("%s\n", baslik);
    for (int k = 0; k <= MAX; k++) {
        if (bos[k] == NULL)
            continue;
        printf("  bos[%d] (%3d KB):", k, 1 << k);
        for (struct dugum *d = bos[k]; d != NULL; d = d->sonraki)
            printf(" @%d", d->adres);
        printf("\n");
    }
}

static int mertebe(int kb)           /* istegi 2'nin bir ust kuvvetine yuvarla */
{
    int k = 0;
    while ((1 << k) < kb)
        k++;
    return k;
}

int main(void)
{
    ekle(MAX, 0);
    listeleri_yaz("Baslangic:");

    int A = ayir(mertebe(21));
    printf("A = @%d (%d KB), ic parcalanma %d KB\n", A, 1 << mertebe(21),
           (1 << mertebe(21)) - 21);
    int B = ayir(mertebe(64));
    printf("B = @%d (%d KB)\n", B, 1 << mertebe(64));
    listeleri_yaz("A ve B'den sonra:");

    birak(A, mertebe(21));
    listeleri_yaz("birak(0, 5): @32 ile birlesti, @64 dolu -> durdu:");
    birak(B, mertebe(64));
    listeleri_yaz("birak(64, 6): 64 -> 128 -> 256 KB:");

    while (bos[MAX] != NULL)
        cek(MAX);
    return 0;
}

/* Beklenen cikti:
 * Baslangic:
 *   bos[8] (256 KB): @0
 * A = @0 (32 KB), ic parcalanma 11 KB
 * B = @64 (64 KB)
 * A ve B'den sonra:
 *   bos[5] ( 32 KB): @32
 *   bos[7] (128 KB): @128
 * birak(0, 5): @32 ile birlesti, @64 dolu -> durdu:
 *   bos[6] ( 64 KB): @0
 *   bos[7] (128 KB): @128
 * birak(64, 6): 64 -> 128 -> 256 KB:
 *   bos[8] (256 KB): @0
 */
