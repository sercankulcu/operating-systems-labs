/* Gunlukten kurtarma (journaling): acilista gunluk taranir; TxB ... TxE ile
 * kesinlesmis islemlerin bloklari son yerlerine yeniden yazilir, TxE'si olmayan
 * islem atilir. Sistem islem 3 yazilirken cokmus: 10 kayit. */
#include <stdio.h>
#include <string.h>

enum { TXB, BLOK, TXE };

typedef struct {
    int tur;
    const char *hedef;          /* BLOK: son yer (I7, B, I9, D40) */
    const char *veri;           /* BLOK: surum */
} Kayit;

/* Diskteki son yerler ve icerikleri */
static struct { const char *ad; const char *surum; } disk[] = {
    {"I7", "v0"}, {"B", "v0"}, {"I9", "v0"}, {"D40", "v0"},
};
#define DISK_BOY (int)(sizeof disk / sizeof disk[0])

static void yaz(const char *hedef, const char *veri)
{
    for (int i = 0; i < DISK_BOY; i++)
        if (strcmp(disk[i].ad, hedef) == 0) {
            disk[i].surum = veri;
            printf("  yaz(%s, %s)\n", hedef, veri);
        }
}

void kurtar(Kayit g[], int n)
{
    int i = 0;
    while (i < n && g[i].tur == TXB) {
        int j = i + 1;
        while (j < n && g[j].tur == BLOK)
            j++;
        if (j == n || g[j].tur != TXE)
            break;              /* onaysiz: at */
        for (int k = i + 1; k < j; k++)
            yaz(g[k].hedef, g[k].veri);
        i = j + 1;          /* sonraki */
    }
    if (i < n)
        printf("  g[%d]'den baslayan islem onaysiz: atildi\n", i);
}

int main(void)
{
    Kayit g[] = {
        {TXB, NULL, NULL}, {BLOK, "I7", "v1"}, {BLOK, "B", "v1"}, {TXE, NULL, NULL},
        {TXB, NULL, NULL}, {BLOK, "I9", "v2"}, {BLOK, "D40", "v2"}, {TXE, NULL, NULL},
        {TXB, NULL, NULL}, {BLOK, "I7", "v3"},
    };
    int n = (int)(sizeof g / sizeof g[0]);

    printf("kurtar(g, %d):\n", n);
    kurtar(g, n);
    printf("son yerler:");
    for (int i = 0; i < DISK_BOY; i++)
        printf(" %s=%s", disk[i].ad, disk[i].surum);
    printf("\n");
    return 0;
}

/* Beklenen cikti:
 * kurtar(g, 10):
 *   yaz(I7, v1)
 *   yaz(B, v1)
 *   yaz(I9, v2)
 *   yaz(D40, v2)
 *   g[8]'den baslayan islem onaysiz: atildi
 * son yerler: I7=v1 B=v1 I9=v2 D40=v2
 */
