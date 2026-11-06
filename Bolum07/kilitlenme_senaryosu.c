/* Kilitlenme nasil olusur / nasil onlenebilir (Tanenbaum Sekil 6-4):
 * A, B, C surecleri R, S, T kaynaklarini istenen sirada ister ve birakir.
 * Ayni programlar iki farkli cizelgeleme sirasiyla adim adim calistirilir. */
#include <stdio.h>

#define NS 3                    /* surec: A, B, C */
#define NK 3                    /* kaynak: R, S, T */

enum { ISTE, BIRAK };

struct islem { int tur; int kaynak; };

/*  A        B        C
 *  R iste   S iste   T iste
 *  S iste   T iste   R iste
 *  R birak  S birak  T birak
 *  S birak  T birak  R birak */
static const struct islem program[NS][4] = {
    { {ISTE, 0}, {ISTE, 1}, {BIRAK, 0}, {BIRAK, 1} },
    { {ISTE, 1}, {ISTE, 2}, {BIRAK, 1}, {BIRAK, 2} },
    { {ISTE, 2}, {ISTE, 0}, {BIRAK, 2}, {BIRAK, 0} },
};

static const char sad[] = "ABC";
static const char kad[] = "RST";

static int pc[NS];              /* siradaki islem */
static int bekledigi[NS];       /* -1: bloke degil */
static int sahip[NK];           /* -1: bos */
static int adim_no;

static void sifirla(void)
{
    adim_no = 0;
    for (int p = 0; p < NS; p++) {
        pc[p] = 0;
        bekledigi[p] = -1;
    }
    for (int k = 0; k < NK; k++)
        sahip[k] = -1;
}

/* p surecinin siradaki islemini yap */
static void adim(int p)
{
    printf("%2d. ", ++adim_no);
    if (pc[p] == 4 || bekledigi[p] != -1) {
        printf("%c calisamaz\n", sad[p]);
        return;
    }

    const struct islem *is = &program[p][pc[p]];
    int k = is->kaynak;

    if (is->tur == ISTE) {
        if (sahip[k] == -1) {
            sahip[k] = p;
            pc[p]++;
            printf("%c, %c'yi ister ve alir.\n", sad[p], kad[k]);
        } else {
            bekledigi[p] = k;
            printf("%c, %c'yi ister: %c, %c'de -> %c bloke.\n",
                   sad[p], kad[k], kad[k], sad[sahip[k]], sad[p]);
        }
        return;
    }
    /* BIRAK: kaynagi bekleyen varsa ona ver */
    sahip[k] = -1;
    pc[p]++;
    printf("%c, %c'yi birakir", sad[p], kad[k]);
    for (int q = 0; q < NS; q++) {
        if (bekledigi[q] == k) {
            sahip[k] = q;
            bekledigi[q] = -1;
            pc[q]++;
            printf(" -> %c, %c'ye verilir", kad[k], sad[q]);
            break;
        }
    }
    printf("%s\n", pc[p] == 4 ? "; biter." : ".");
}

static void sonuc(void)
{
    int biten = 0, bloke = 0;

    for (int p = 0; p < NS; p++) {
        if (pc[p] == 4)
            biten++;
        else if (bekledigi[p] != -1)
            bloke++;
    }
    if (bloke == NS)
        printf("Hepsi bloke, dongu kapandi: KILITLENME\n\n");
    else
        printf("Biten: %d, bloke: %d -> kilitlenme yok\n\n", biten, bloke);
}

static void calistir(const char *baslik, const char *sira)
{
    printf("%s\n", baslik);
    sifirla();
    for (const char *c = sira; *c; c++)
        adim(*c - 'A');
    sonuc();
}

int main(void)
{
    calistir("Istekler geldigi sirada (A B C A B C):", "ABCABC");
    calistir("IS, B'yi bir sure bekletir:", "ACACAABBCCBB");
    return 0;
}

/* Beklenen cikti:
 * Istekler geldigi sirada (A B C A B C):
 *  1. A, R'yi ister ve alir.
 *  2. B, S'yi ister ve alir.
 *  3. C, T'yi ister ve alir.
 *  4. A, S'yi ister: S, B'de -> A bloke.
 *  5. B, T'yi ister: T, C'de -> B bloke.
 *  6. C, R'yi ister: R, A'de -> C bloke.
 * Hepsi bloke, dongu kapandi: KILITLENME
 *
 * IS, B'yi bir sure bekletir:
 *  1. A, R'yi ister ve alir.
 *  2. C, T'yi ister ve alir.
 *  3. A, S'yi ister ve alir.
 *  4. C, R'yi ister: R, A'de -> C bloke.
 *  5. A, R'yi birakir -> R, C'ye verilir.
 *  6. A, S'yi birakir; biter.
 *  7. B, S'yi ister ve alir.
 *  8. B, T'yi ister: T, C'de -> B bloke.
 *  9. C, T'yi birakir -> T, B'ye verilir.
 * 10. C, R'yi birakir; biter.
 * 11. B, S'yi birakir.
 * 12. B, T'yi birakir; biter.
 * Biten: 3, bloke: 0 -> kilitlenme yok
 */
