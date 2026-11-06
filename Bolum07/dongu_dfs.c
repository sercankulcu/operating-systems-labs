/* Bekleme cizgesinde dongu arama: renkli DFS (beyaz/gri/siyah).
 * Gri bir dugume giden kenar geri kenardir: dongu, yani kilitlenme var.
 * Cizge: P1 -> P2, P2 -> P3 ve P4, P4 -> P5, P5 -> P2. */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define N 5

enum { BEYAZ, GRI, SIYAH };
int renk[N];          /* hepsi BEYAZ */
bool bekler[N][N];    /* bekler[u][v]: Pu, Pv'nin tuttugu kaynagi bekliyor */

bool dongu(int u)
{
    renk[u] = GRI;      /* yolda */
    for (int v = 0; v < N; v++) {
        if (!bekler[u][v]) continue;
        if (renk[v] == GRI)
            return true;    /* geri kenar */
        if (renk[v] == BEYAZ && dongu(v))
            return true;
    }
    renk[u] = SIYAH;    /* bitti */
    return false;
}

static void kenar(int u, int v)          /* P numaralari 1'den baslar */
{
    bekler[u - 1][v - 1] = true;
}

static void renkleri_yaz(void)
{
    static const char harf[] = "BGS";

    for (int i = 0; i < N; i++)
        printf("P%d=%c%s", i + 1, harf[renk[i]], i + 1 < N ? " " : "\n");
}

static void dene(const char *baslik)
{
    bool var;

    memset(renk, 0, sizeof renk);            /* BEYAZ = 0 */
    var = dongu(0);
    printf("%s\ndongu(P1) = %s\n", baslik, var ? "true" : "false");
    renkleri_yaz();
}

int main(void)
{
    kenar(1, 2);
    kenar(2, 3);
    kenar(2, 4);
    kenar(4, 5);
    kenar(5, 2);
    dene("Cizge 1: P1->P2, P2->P3, P2->P4, P4->P5, P5->P2");
    printf("geri kenar P5 -> P2: dongu P2 -> P4 -> P5 -> P2 kilitlenmis;\n"
           "P1 dongude degil ama P2'yi bekledigi icin takili.\n\n");

    bekler[4][1] = false;                    /* P5 -> P2 kenarini kaldir */
    dene("Cizge 2: P5->P2 kenari yok");
    printf("dongu yok: kilitlenme yok\n");
    return 0;
}

/* Beklenen cikti:
 * Cizge 1: P1->P2, P2->P3, P2->P4, P4->P5, P5->P2
 * dongu(P1) = true
 * P1=G P2=G P3=S P4=G P5=G
 * geri kenar P5 -> P2: dongu P2 -> P4 -> P5 -> P2 kilitlenmis;
 * P1 dongude degil ama P2'yi bekledigi icin takili.
 *
 * Cizge 2: P5->P2 kenari yok
 * dongu(P1) = false
 * P1=S P2=S P3=S P4=S P5=S
 * dongu yok: kilitlenme yok
 */
