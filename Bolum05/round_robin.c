/* Round Robin (q = 4): bastaki surec en fazla q kadar calisir; bitmediyse
 * kuyrugun sonuna doner. Dilim sirasinda gelenler kesilen surecten ONCE
 * kuyruga eklenir. Cikti: her surecin bitis ani. */
#include <stdio.h>

typedef struct {
    const char *ad;
    int gelis, sure, kalan;
} Surec;

/* ---- hazir kuyrugu (dairesel dizi) ---- */
#define KAP 16
static Surec *kuyruk[KAP];
static int bas = 0, adet = 0;

static int bos(void) { return adet == 0; }

static void ekle(Surec *s)
{
    kuyruk[(bas + adet) % KAP] = s;
    adet++;
}

static Surec *cikar(void)
{
    Surec *s = kuyruk[bas];
    bas = (bas + 1) % KAP;
    adet--;
    return s;
}

static int min(int x, int y) { return x < y ? x : y; }

/* t anina kadar gelmis ve henuz eklenmemis surecleri kuyruga ekler. */
static void gelenleri_ekle(Surec p[], int n, int *k, int t)
{
    while (*k < n && p[*k].gelis <= t) {
        ekle(&p[*k]);
        (*k)++;
    }
}

/* p[] gelis sirasina gore sirali */
void rr(Surec p[], int n, int q)
{
    int t = 0, k = 0;     /* k: siradaki */
    gelenleri_ekle(p, n, &k, t);
    while (!bos()) {
        Surec *s = cikar();  /* bastaki */
        int dilim = min(q, s->kalan);
        t += dilim; s->kalan -= dilim;
        gelenleri_ekle(p, n, &k, t); /* once */
        if (s->kalan > 0) ekle(s);   /* sonra */
        else printf("%s %d\n", s->ad, t);
    }
}

int main(void)
{
    Surec p[] = {
        {"P1", 0, 8, 8},
        {"P2", 1, 4, 4},
        {"P3", 2, 9, 9},
        {"P4", 3, 5, 5},
    };
    rr(p, 4, 4);
    return 0;
}

/* Beklenen cikti (surec ve bitis ani):
 * P2 8
 * P1 20
 * P4 25
 * P3 26
 */
