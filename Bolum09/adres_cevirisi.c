/* Talebe bagli sayfalamada adres cevirisi: once TLB, iskada sayfa tablosu,
 * sayfa bellekte degilse sayfa hatasi (diskten bos cerceveye okuma).
 * Baslangic: TLB'de (0,1) ve (1,4); sayfa 2 cerceve 5'te; sayfa 3 diskte. */
#include <stdio.h>

#define SAYFA 16
#define TLB_BOY 4

struct pt_girdi { int gecerli, cerceve, R; };
struct tlb_girdi { int dolu, p, f; };

static struct pt_girdi pt[SAYFA] = {
    [0] = {1, 1, 0}, [1] = {1, 4, 0}, [2] = {1, 5, 0}, [3] = {0, -1, 0},
};
static struct tlb_girdi tlb[TLB_BOY] = { {1, 0, 1}, {1, 1, 4} };
static int tlb_sira = 2;        /* FIFO ile doldurulacak sonraki yuva */
static int bos_cerceve = 7;
static int isabet, iska, hata;

static int tlb_ara(int p)
{
    for (int i = 0; i < TLB_BOY; i++)
        if (tlb[i].dolu && tlb[i].p == p) {
            isabet++;
            return tlb[i].f;
        }
    iska++;
    return -1;
}

static void tlb_ekle(int p, int f)
{
    tlb[tlb_sira] = (struct tlb_girdi){1, p, f};
    tlb_sira = (tlb_sira + 1) % TLB_BOY;
}

static void sayfa_hatasi(int p)
{
    hata++;
    printf("  sayfa hatasi: sayfa %d diskten cerceve %d'ye okunuyor\n", p, bos_cerceve);
    pt[p].cerceve = bos_cerceve++;
    pt[p].gecerli = 1;
}

int ceviri(int va)
{
    int p = va >> 12;      /* sayfa no */
    int d = va & 0xFFF;    /* bagil konum */
    int f = tlb_ara(p);    /* -1: iska */
    if (f < 0) {
        if (!pt[p].gecerli)  /* bellekte yok */
            sayfa_hatasi(p);   /* diskten al */
        f = pt[p].cerceve;
        tlb_ekle(p, f);
    }
    pt[p].R = 1;
    return (f << 12) | d;
}

int main(void)
{
    const int va[3] = {0x2A10, 0x2B04, 0x3008};

    for (int i = 0; i < 3; i++) {
        int pa = ceviri(va[i]);
        printf("0x%X -> 0x%X\n", (unsigned)va[i], (unsigned)pa);
    }
    printf("TLB isabet: %d, iska: %d, sayfa hatasi: %d\n", isabet, iska, hata);
    return 0;
}

/* Beklenen cikti:
 * 0x2A10 -> 0x5A10
 * 0x2B04 -> 0x5B04
 *   sayfa hatasi: sayfa 3 diskten cerceve 7'ye okunuyor
 * 0x3008 -> 0x7008
 * TLB isabet: 1, iska: 2, sayfa hatasi: 1
 */
