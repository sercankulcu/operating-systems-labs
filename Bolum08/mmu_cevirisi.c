/* MMU icinde ceviri (Tanenbaum Sekil 3-9/3-10): 16 bit sanal adres, 4 KiB sayfa.
 * Ust 4 bit sayfa numarasi, alt 12 bit aynen kopyalanir. Sunumdaki komutlar:
 * MOV REG, 0 / 8192 / 20500 / 8196 / 32780 */
#include <stdint.h>
#include <stdio.h>

#define SAYFA_SAYISI 16

struct girdi {
    int mevcut;                 /* mevcut/yok biti */
    unsigned cerceve;           /* 3 bit: 8 cerceve, 32 KiB fiziksel */
};

/* sayfa tablosu (sayfa -> cerceve): 0->2 1->1 2->6 3->0 4->4 5->3 9->5 11->7 */
static const struct girdi tablo[SAYFA_SAYISI] = {
    [0] = {1, 2}, [1] = {1, 1}, [2] = {1, 6}, [3] = {1, 0},
    [4] = {1, 4}, [5] = {1, 3}, [9] = {1, 5}, [11] = {1, 7},
};

/* Donus: fiziksel adres, sayfa yoksa -1 (sayfa hatasi) */
static long mmu_cevir(uint16_t sanal)
{
    unsigned p = sanal >> 12;
    unsigned d = sanal & 0xFFF;

    printf("sanal %5u: p = %2u, d = %4u -> ", (unsigned)sanal, p, d);
    if (!tablo[p].mevcut) {
        printf("sayfa %u eslenmemis: SAYFA HATASI\n", p);
        return -1;
    }
    long fiziksel = ((long)tablo[p].cerceve << 12) | d;
    printf("cerceve %u, fiziksel %ld\n", tablo[p].cerceve, fiziksel);
    return fiziksel;
}

int main(void)
{
    const uint16_t komut[] = {0, 8192, 20500, 8196, 32780};   /* MOV REG, adres */

    for (size_t i = 0; i < sizeof komut / sizeof komut[0]; i++)
        mmu_cevir(komut[i]);
    return 0;
}

/* Beklenen cikti:
 * sanal     0: p =  0, d =    0 -> cerceve 2, fiziksel 8192
 * sanal  8192: p =  2, d =    0 -> cerceve 6, fiziksel 24576
 * sanal 20500: p =  5, d =   20 -> cerceve 3, fiziksel 12308
 * sanal  8196: p =  2, d =    4 -> cerceve 6, fiziksel 24580
 * sanal 32780: p =  8, d =   12 -> sayfa 8 eslenmemis: SAYFA HATASI
 */
