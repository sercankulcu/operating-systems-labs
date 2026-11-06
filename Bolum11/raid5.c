/* RAID 5 kucuk yazma ve kurtarma: tek bir veri blogu degisirken yalniz eski
 * veri ve eski eslik okunur (4 G/C); bozulan disk kalanlarin XOR'u olarak
 * yeniden kurulur. Bir serit: D0, D1, D2 ve eslik P (birer bayt). */
#include <stdio.h>
#include <stdint.h>

#define P 3                   /* eslik diskinin numarasi */

static uint8_t disk[P + 1] = { 0xB2, 0x69, 0xC5, 0x1E };   /* D0 D1 D2 P */
static int gc_sayisi = 0;

static uint8_t oku(int d)
{
    gc_sayisi++;
    return disk[d];
}

static void yaz(int d, uint8_t v)
{
    gc_sayisi++;
    disk[d] = v;
}

static const char *ikili(uint8_t v)
{
    static char s[2][10];
    static int sira = 0;
    char *b = s[sira];
    sira ^= 1;
    for (int i = 0; i < 8; i++) {
        b[i + (i >= 4)] = (char)('0' + ((v >> (7 - i)) & 1));
    }
    b[4] = ' ';
    b[9] = '\0';
    return b;
}

void kucuk_yaz(int d, uint8_t yeni)
{
    uint8_t eski = oku(d);   /* G/C 1 */
    uint8_t p = oku(P);      /* G/C 2 */
    p = p ^ eski ^ yeni;
    yaz(d, yeni);            /* G/C 3 */
    yaz(P, p);               /* G/C 4 */
}

uint8_t kur(int bozuk)
{
    uint8_t x = 0;
    for (int d = 0; d <= P; d++)
        if (d != bozuk)
            x ^= oku(d);         /* P dahil */
    return x;
}

int main(void)
{
    printf("P  = %s (D0 ^ D1 ^ D2)\n", ikili(disk[P]));

    kucuk_yaz(1, 0x33);
    printf("kucuk_yaz(1, %s): %d G/C, yeni P = %s\n",
           ikili(0x33), gc_sayisi, ikili(disk[P]));

    uint8_t kayip = disk[2];
    disk[2] = 0;             /* disk 2 bozuldu */
    gc_sayisi = 0;
    uint8_t x = kur(2);
    printf("kur(2) = %s, %d okuma, %s\n", ikili(x), gc_sayisi,
           x == kayip ? "kaybolan D2 ile ayni" : "HATA");
    return 0;
}

/* Beklenen cikti:
 * P  = 0001 1110 (D0 ^ D1 ^ D2)
 * kucuk_yaz(1, 0011 0011): 4 G/C, yeni P = 0100 0100
 * kur(2) = 1100 0101, 3 okuma, kaybolan D2 ile ayni
 */
