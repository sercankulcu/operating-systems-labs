/* Linda demet uzayi: in() sablona uyan ilk demeti bulur, bicimsel alanlari
 * doldurur ve demeti uzaydan cikarir; uyan yoksa surec bloke olur (burada
 * NULL doner). Ornek: in("abc", 2, ? int i). */
#include <stdio.h>
#include <string.h>

#define MAXALAN 4
enum { TAM, DIZGI };            /* alan turleri */

typedef struct Deger { int i; const char *s; } Deger;

typedef struct Demet {
    const char *ad;             /* izleme icin: d1, d2, ... */
    int n;                      /* alan sayisi */
    int tur[MAXALAN];
    Deger v[MAXALAN];
    struct Demet *snr;          /* sonraki demet */
} Demet;

typedef struct Sablon {
    int n;
    int tur[MAXALAN];
    int sabit[MAXALAN];         /* 1: gercek deger, 0: bicimsel (? int i) */
    Deger v[MAXALAN];
} Sablon;

Demet *uzay = NULL;

static int esit(Demet *d, Sablon *s, int k)
{
    if (d->tur[k] == TAM)
        return d->v[k].i == s->v[k].i;
    return strcmp(d->v[k].s, s->v[k].s) == 0;
}

static void cikar(Demet *d)
{
    Demet **p = &uzay;
    while (*p && *p != d)
        p = &(*p)->snr;
    if (*p)
        *p = d->snr;
}

Demet *in_ara(Sablon *s)
{
    for (Demet *d = uzay; d; d = d->snr) {
        if (d->n != s->n) continue;
        int ok = 1;
        for (int k = 0; ok && k < s->n; k++)
            if (d->tur[k] != s->tur[k] ||
                (s->sabit[k] && !esit(d, s, k)))
                ok = 0;          /* alan uymadi */
        if (ok) { cikar(d); return d; }
    }
    return NULL;           /* bloke ol */
}

int main(void)
{
    static Demet d1 = { "d1", 2, { DIZGI, TAM },      { { 0, "abc" }, { 2, NULL } },              NULL };
    static Demet d2 = { "d2", 3, { DIZGI, TAM, TAM }, { { 0, "xyz" }, { 2, NULL }, { 7, NULL } }, NULL };
    static Demet d3 = { "d3", 3, { DIZGI, TAM, TAM }, { { 0, "abc" }, { 1, NULL }, { 4, NULL } }, NULL };
    static Demet d4 = { "d4", 3, { DIZGI, TAM, TAM }, { { 0, "abc" }, { 2, NULL }, { 5, NULL } }, NULL };
    static Demet d5 = { "d5", 3, { DIZGI, TAM, TAM }, { { 0, "abc" }, { 2, NULL }, { 9, NULL } }, NULL };
    d1.snr = &d2; d2.snr = &d3; d3.snr = &d4; d4.snr = &d5;
    uzay = &d1;

    /* in("abc", 2, ? int i) */
    Sablon s = { 3, { DIZGI, TAM, TAM }, { 1, 1, 0 },
                 { { 0, "abc" }, { 2, NULL }, { 0, NULL } } };

    for (int c = 1; c <= 3; c++) {
        Demet *d = in_ara(&s);
        if (d) {
            int i = d->v[2].i;       /* bicimsel alan doldurulur */
            printf("%d. in: i = %d (%s)\n", c, i, d->ad);
        } else {
            printf("%d. in: uyan demet yok, surec bloke olurdu\n", c);
        }
    }
    return 0;
}

/* Beklenen cikti:
 * 1. in: i = 5 (d4)
 * 2. in: i = 9 (d5)
 * 3. in: uyan demet yok, surec bloke olurdu
 */
