/* Saat (ikinci sans) sayfa yer degistirme: el R = 1 olan cercevelerin R'sini
 * sifirlayip ilerler, ilk R = 0 cerceve kurbandir. Kirli (M = 1) kurban once
 * diske yazilir. Dort cerceve, el cerceve 2'de; hata(9) ve hata(4). */
#include <stdio.h>

#define N 4

struct cerceve { int sayfa, R, M; };

static struct cerceve cer[N] = {
    {3, 1, 0}, {8, 0, 1}, {5, 1, 0}, {6, 1, 1},
};
static int el = 2;
static int yazilan = 0;

static void diske_yaz(int k)
{
    (void)k;
    yazilan++;
}

int kurban_sec(void)
{
    while (cer[el].R == 1) { /* 2. sans */
        cer[el].R = 0;
        el = (el + 1) % N;
    }
    int k = el;              /* kurban */
    el = (el + 1) % N;
    return k;
}

void hata(int p)
{
    int k = kurban_sec();
    if (cer[k].M) diske_yaz(k); /* kirli */
    cer[k].sayfa = p; cer[k].R = 1;
    cer[k].M = 0;
}

static void yaz(void)
{
    for (int i = 0; i < N; i++)
        printf("  c%d: s%d R=%d M=%d\n", i, cer[i].sayfa, cer[i].R, cer[i].M);
    printf("  el = %d, diske yazilan: %d\n", el, yazilan);
}

static void hata_ve_raporla(int p)
{
    struct cerceve once[N];

    for (int i = 0; i < N; i++)
        once[i] = cer[i];
    hata(p);
    for (int k = 0; k < N; k++)
        if (cer[k].sayfa == p)
            printf("hata(%d): kurban c%d (s%d, %s)\n", p, k, once[k].sayfa,
                   once[k].M ? "kirli" : "temiz");
}

int main(void)
{
    printf("Baslangic:\n");
    yaz();
    hata_ve_raporla(9);
    hata_ve_raporla(4);
    printf("Son durum:\n");
    yaz();
    return 0;
}

/* Beklenen cikti:
 * Baslangic:
 *   c0: s3 R=1 M=0
 *   c1: s8 R=0 M=1
 *   c2: s5 R=1 M=0
 *   c3: s6 R=1 M=1
 *   el = 2, diske yazilan: 0
 * hata(9): kurban c1 (s8, kirli)
 * hata(4): kurban c2 (s5, temiz)
 * Son durum:
 *   c0: s3 R=0 M=0
 *   c1: s9 R=1 M=0
 *   c2: s4 R=1 M=0
 *   c3: s6 R=0 M=1
 *   el = 3, diske yazilan: 1
 */
