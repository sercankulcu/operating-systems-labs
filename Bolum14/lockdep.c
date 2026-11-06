/* lockdep kilit sirasi cizgesi: "H tutulurken L alindi" iliskisi H -> L
 * kenari olarak birikir. Yeni kenar bir cevrim kapatirsa (L'den H'ye yol
 * varsa) olasi kilitlenme uyarisi verilir; gorevler hic ayni anda
 * calismasa bile. Gorevler ve kilit siniflari kullanici alaninda benzetilir. */
#include <stdio.h>
#include <string.h>

#define NKILIT 3
static const char *ad[NKILIT] = { "A", "B", "C" };
enum { A, B, C };

typedef struct {
    const char *ad;
    int tutulan[8];
    int n;
} task;

static int kenar[NKILIT][NKILIT];     /* kenar[H][L]: H, L'den once */
static int onceki[NKILIT];            /* bulunan yolu yazdirmak icin */

/* L'den H'ye yol var mi? (derinlik oncelikli arama) */
static int yol_ara(int u, int H, int *gorulen)
{
    if (u == H)
        return 1;
    gorulen[u] = 1;
    for (int v = 0; v < NKILIT; v++)
        if (kenar[u][v] && !gorulen[v]) {
            onceki[v] = u;
            if (yol_ara(v, H, gorulen))
                return 1;
        }
    return 0;
}

static int yol_var(int L, int H)
{
    int gorulen[NKILIT] = { 0 };
    return yol_ara(L, H, gorulen);
}

static void kenar_ekle(int H, int L)
{
    kenar[H][L] = 1;
}

static void uyar(const char *msg, int H, int L)
{
    int yol[NKILIT + 1], n = 0;
    for (int u = H; u != L; u = onceki[u])
        yol[n++] = u;
    yol[n++] = L;
    printf("WARNING: possible circular locking dependency (%s)\n", msg);
    printf("  %s -> %s, ama ", ad[H], ad[L]);
    for (int i = n - 1; i >= 0; i--)
        printf("%s%s", ad[yol[i]], i ? " -> " : "\n");
}

void lock_acquire(task *t, int L)
{
    printf("%s: %s alir\n", t->ad, ad[L]);
    for (int i = 0; i < t->n; i++) {
        int H = t->tutulan[i];
        if (yol_var(L, H))   /* L -> ... -> H */
            uyar("olasi kilitlenme", H, L);
        kenar_ekle(H, L);    /* H, L'den once */
    }
    t->tutulan[t->n++] = L;
}

void lock_release(task *t) { t->n--; }

int main(void)
{
    /* T1: A,B   T2: B,C   T3: C,A */
    task T1 = { "T1", { 0 }, 0 }, T2 = { "T2", { 0 }, 0 }, T3 = { "T3", { 0 }, 0 };

    lock_acquire(&T1, A); lock_acquire(&T1, B);
    lock_release(&T1);    lock_release(&T1);

    lock_acquire(&T2, B); lock_acquire(&T2, C);
    lock_release(&T2);    lock_release(&T2);

    lock_acquire(&T3, C); lock_acquire(&T3, A);
    lock_release(&T3);    lock_release(&T3);
    return 0;
}

/* Beklenen cikti:
 * T1: A alir
 * T1: B alir
 * T2: B alir
 * T2: C alir
 * T3: C alir
 * T3: A alir
 * WARNING: possible circular locking dependency (olasi kilitlenme)
 *   C -> A, ama A -> B -> C
 */
