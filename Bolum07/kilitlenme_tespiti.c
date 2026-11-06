/* Cok ornekli kaynaklarda kilitlenme tespiti (Tanenbaum Sekil 6-7).
 * Istegi (Ri) bos kaynaklarla (A) karsilanabilen surec biter ve tuttuklarini (Ci)
 * geri verir; sonda isaretsiz kalan surecler kilitlenmistir. */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define NP 3            /* surec */
#define M 4             /* kaynak turu: teyp, cizici, tarayici, CD */

static void vektor_yaz(const int v[M])
{
    printf("(%d %d %d %d)", v[0], v[1], v[2], v[3]);
}

static bool kucuk_esit(const int r[M], const int a[M])
{
    for (int j = 0; j < M; j++)
        if (r[j] > a[j])
            return false;
    return true;
}

/* A <- bos kaynak vektoru; tum Pi isaretsiz; tekrarla: Ri <= A olan
 * isaretsiz bir Pi bul, bulunamazsa cik; A <- A + Ci; Pi'yi isaretle.
 * Donus: kilitlenen surec sayisi. */
static int tespit(const int bos[M], int C[NP][M], int R[NP][M])
{
    int A[M];
    bool isaretli[NP] = { false };
    int kilitli = 0;

    memcpy(A, bos, sizeof A);
    for (;;) {
        int i;
        for (i = 0; i < NP; i++)
            if (!isaretli[i] && kucuk_esit(R[i], A))
                break;
        if (i == NP)
            break;                          /* bulunamadi: cik */
        for (int j = 0; j < M; j++)
            A[j] += C[i][j];                /* Pi biter, birakir */
        isaretli[i] = true;
        printf("P%d: R%d = ", i + 1, i + 1);
        vektor_yaz(R[i]);
        printf(" <= A -> biter, A = ");
        vektor_yaz(A);
        printf("\n");
    }
    for (int i = 0; i < NP; i++) {
        if (!isaretli[i]) {
            printf("P%d isaretsiz: kilitlenmis\n", i + 1);
            kilitli++;
        }
    }
    if (kilitli == 0)
        printf("Tum surecler isaretlendi: kilitlenme yok\n");
    return kilitli;
}

int main(void)
{
    const int A[M] = { 2, 1, 0, 0 };        /* E = (4 2 3 1) */
    int C[NP][M] = {                        /* tahsis matrisi */
        { 0, 0, 1, 0 },
        { 2, 0, 0, 1 },
        { 0, 1, 2, 0 },
    };
    int R[NP][M] = {                        /* istek matrisi */
        { 2, 0, 0, 1 },
        { 1, 0, 1, 0 },
        { 2, 1, 0, 0 },
    };

    printf("Ornek: A = (2 1 0 0)\n");
    tespit(A, C, R);

    printf("\nDegisik durum: P3 ayrica bir CD isterse, R3 = (2 1 0 1)\n");
    R[2][3] = 1;
    tespit(A, C, R);
    return 0;
}

/* Beklenen cikti:
 * Ornek: A = (2 1 0 0)
 * P3: R3 = (2 1 0 0) <= A -> biter, A = (2 2 2 0)
 * P2: R2 = (1 0 1 0) <= A -> biter, A = (4 2 2 1)
 * P1: R1 = (2 0 0 1) <= A -> biter, A = (4 2 3 1)
 * Tum surecler isaretlendi: kilitlenme yok
 *
 * Degisik durum: P3 ayrica bir CD isterse, R3 = (2 1 0 1)
 * P1 isaretsiz: kilitlenmis
 * P2 isaretsiz: kilitlenmis
 * P3 isaretsiz: kilitlenmis
 */
