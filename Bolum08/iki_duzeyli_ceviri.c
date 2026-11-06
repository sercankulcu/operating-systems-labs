/* Iki duzeyli sayfa tablosu ile ceviri: 32 bit adres = 10 + 10 + 12 bit.
 * Ust tabloda yalniz girdi 2 gecerli; onun ikinci duzey tablosunda girdi 1 ->
 * cerceve 0x2F. sayfa_hatasi() MMU tuzagini setjmp/longjmp ile taklit eder. */
#include <inttypes.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>

typedef struct pte {
    unsigned p;                 /* mevcut biti */
    uint32_t cerceve;           /* ikinci duzey: fiziksel cerceve */
    struct pte *tablo;          /* ust duzey: ikinci duzey tablonun yeri */
} pte_t;

static pte_t ust[1024];         /* ust (sayfa dizini) tablo */
static pte_t ikinci[1024];      /* ust[2]'nin gosterdigi ikinci duzey tablo */
static jmp_buf tuzak;

static _Noreturn void sayfa_hatasi(uint32_t va)
{
    printf("0x%08" PRIX32 " -> SAYFA HATASI (cekirdege tuzak)\n", va);
    longjmp(tuzak, 1);
}

/* 32 bit: 10 + 10 + 12 bit */
uint32_t cevir(uint32_t va)
{
    uint32_t p1 = va >> 22;
    uint32_t p2 = (va >> 12) & 0x3FF;
    uint32_t d  = va & 0xFFF;
    printf("va = 0x%08" PRIX32 ": p1 = %" PRIu32 ", p2 = %" PRIu32
           ", d = 0x%03" PRIX32 "\n", va, p1, p2, d);
    pte_t e1 = ust[p1];
    if (!e1.p) sayfa_hatasi(va);
    pte_t e2 = e1.tablo[p2];
    if (!e2.p) sayfa_hatasi(va);
    return (e2.cerceve << 12) | d;
}

static const uint32_t adres[2] = { 0x00801A3C, 0x01403004 };
static int sira;                /* longjmp'ten etkilenmesin diye yerel degil */

static void dene(void)
{
    if (setjmp(tuzak) == 0) {
        uint32_t pa = cevir(adres[sira]);
        printf("0x%08" PRIX32 " -> 0x%08" PRIX32 "\n", adres[sira], pa);
    }
}

int main(void)
{
    ust[2].p = 1;
    ust[2].tablo = ikinci;
    ikinci[1].p = 1;
    ikinci[1].cerceve = 0x2F;

    for (sira = 0; sira < 2; sira++)
        dene();
    return 0;
}

/* Beklenen cikti:
 * va = 0x00801A3C: p1 = 2, p2 = 1, d = 0xA3C
 * 0x00801A3C -> 0x0002FA3C
 * va = 0x01403004: p1 = 5, p2 = 3, d = 0x004
 * 0x01403004 -> SAYFA HATASI (cekirdege tuzak)
 */
