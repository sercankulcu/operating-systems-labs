/* Sayfa hatasi sikligi (PFF): her hatada son hatadan beri gecen sureye bakilir.
 * Aralik C'den buyukse (seyrek hata) son hatadan beri kullanilmayan (R = 0)
 * sayfalar cikarilir; degilse kume buyur. C = 2, 14 referans. */
#include <stdio.h>

#define C 2
#define MAKS 16

typedef struct { int sayfa, R; } Girdi;

typedef struct {
    Girdi kume[MAKS];
    int n;                      /* kumedeki sayfa sayisi */
    int son;                    /* son hatanin zamani */
} Surec;

static void cikar(Surec *s, int i)
{
    printf(" s%d cikarildi;", s->kume[i].sayfa);
    for (int j = i; j < s->n - 1; j++)
        s->kume[j] = s->kume[j + 1];
    s->n--;
}

static void ekle(Surec *s, int p)
{
    s->kume[s->n].sayfa = p;
    s->kume[s->n].R = 1;
    s->n++;
}

void pff(Surec *s, int p, int t)
{
    if (t - s->son > C) {   /* seyrek hata */
        for (int i = s->n - 1; i >= 0; i--)
            if (s->kume[i].R == 0)
                cikar(s, i);    /* kullanilmadi */
    }
    for (int i = 0; i < s->n; i++)
        s->kume[i].R = 0;     /* sifirla */
    ekle(s, p);             /* R = 1 ile */
    s->son = t;
}

static void kume_yaz(const Surec *s)
{
    printf(" kume = {");
    for (int i = 0; i < s->n; i++)
        printf("%ss%d", i ? " " : "", s->kume[i].sayfa);
    printf("}\n");
}

int main(void)
{
    const int ref[15] = {0, 1, 2, 1, 3, 1, 3, 1, 3, 4, 4, 5, 4, 5, 2};  /* ref[t], t = 1..14 */
    Surec s = { .n = 0, .son = 0 };
    int hata = 0;

    for (int t = 1; t <= 14; t++) {
        int p = ref[t], bulundu = 0;
        for (int i = 0; i < s.n; i++)
            if (s.kume[i].sayfa == p) {
                s.kume[i].R = 1;
                bulundu = 1;
            }
        if (bulundu)
            continue;                       /* isabet */
        hata++;
        printf("t=%2d s%d HATA, aralik %d:", t, p, t - s.son);
        pff(&s, p, t);
        kume_yaz(&s);
    }
    printf("14 referansta %d hata\n", hata);
    return 0;
}

/* Beklenen cikti:
 * t= 1 s1 HATA, aralik 1: kume = {s1}
 * t= 2 s2 HATA, aralik 1: kume = {s1 s2}
 * t= 4 s3 HATA, aralik 2: kume = {s1 s2 s3}
 * t= 9 s4 HATA, aralik 5: s2 cikarildi; kume = {s1 s3 s4}
 * t=11 s5 HATA, aralik 2: kume = {s1 s3 s4 s5}
 * t=14 s2 HATA, aralik 3: s3 cikarildi; s1 cikarildi; kume = {s4 s5 s2}
 * 14 referansta 6 hata
 */
