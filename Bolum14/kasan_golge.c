/* KASAN golge bellek denetimi: her 8 baytlik bolge icin 1 golge bayt.
 * 0: hepsi gecerli, 1..7: ilk k bayt gecerli, < 0: kirmizi bolge ya da
 * serbest. Kullanici alaninda kucuk bir "slab" (24 bayt) ve golgesi
 * benzetilir; her erisimden once kasan_denetle cagrilir (derleyicinin
 * ekledigi denetim gibi). */
#include <stdio.h>
#include <stdint.h>

#define KASAN_SERBEST    ((int8_t)0xfb)   /* -5 */
#define KASAN_KIRMIZI    ((int8_t)0xfc)   /* -4 */

static _Alignas(8) unsigned char slab[24];   /* 16 baytlik yuva + kirmizi bolge */
static int8_t golge[sizeof slab / 8];
static uintptr_t GOLGE_OFS;
static unsigned char *p;                     /* rapor icin nesnenin basi */

static void rapor(void *a, int s)
{
    printf("BUG: KASAN: %s p+%td\n",
           s == KASAN_SERBEST ? "use-after-free" : "slab-out-of-bounds",
           (unsigned char *)a - p);
}

void kasan_denetle(void *a, int boy)
{
    int8_t s = *(int8_t *)(((uintptr_t)a >> 3)
                           + GOLGE_OFS);
    if (s == 0) return;            /* hizli yol */
    if ((int)((uintptr_t)a & 7) + boy - 1 >= s)
        rapor(a, s);
}

static void golge_yaz(const char *an)
{
    printf("%-14s golge =", an);
    for (size_t i = 0; i < sizeof golge; i++)
        printf(" %02x", (unsigned)(uint8_t)golge[i]);
    printf("\n");
}

static void *kmalloc(int n)
{
    /* tek yuvali ayirici: 16 baytlik yuva, ardindan kirmizi bolge */
    for (size_t i = 0; i < sizeof golge; i++)
        golge[i] = KASAN_KIRMIZI;
    for (int i = 0; i < n / 8; i++)
        golge[i] = 0;
    if (n % 8)
        golge[n / 8] = (int8_t)(n % 8);
    return slab;
}

static void kfree(void *q)
{
    (void)q;
    golge[0] = golge[1] = KASAN_SERBEST;   /* yuva karantinada */
}

/* derleyicinin ekledigi denetim + okuma */
#define OKU(q, i) (kasan_denetle(&(q)[i], 1), (q)[i])

int main(void)
{
    GOLGE_OFS = (uintptr_t)golge - ((uintptr_t)slab >> 3);
    volatile unsigned char v;

    /* p = kmalloc(13); p[3]; p[12]; p[13]; */
    p = kmalloc(13);
    golge_yaz("kmalloc(13):");
    v = OKU(p, 3);
    v = OKU(p, 12);
    v = OKU(p, 13);
    /* kfree(p); p[0]; */
    kfree(p);
    golge_yaz("kfree(p):");
    v = OKU(p, 0);
    (void)v;
    return 0;
}

/* Beklenen cikti:
 * kmalloc(13):   golge = 00 05 fc
 * BUG: KASAN: slab-out-of-bounds p+13
 * kfree(p):      golge = fb fb fc
 * BUG: KASAN: use-after-free p+0
 * Not: konum (int) ile cevrildigi icin karsilastirma isaretlidir; negatif
 * golge (kirmizi bolge ya da serbest) her konumda raporlanir. Cevirme
 * olmasaydi uintptr_t yuzunden karsilastirma isaretsiz olur ve serbest
 * bellek hic raporlanmazdi.
 */
