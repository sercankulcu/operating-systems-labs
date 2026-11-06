/* Bankaci algoritmasi: guvenlik testi ve istek algoritmasi
 * (Silberschatz 10e 8.6.3 ornegi: A = 10, B = 5, C = 7 ornek, T0..T4).
 * Need = Max - Allocation; istek ancak sonuc durum guvenliyse verilir. */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define N 5             /* is parcacigi */
#define M 3             /* kaynak turu */

static int Available[M] = { 3, 3, 2 };
static int Max[N][M] = {
    { 7, 5, 3 }, { 3, 2, 2 }, { 9, 0, 2 }, { 2, 2, 2 }, { 4, 3, 3 },
};
static int Allocation[N][M] = {
    { 0, 1, 0 }, { 2, 0, 0 }, { 3, 0, 2 }, { 2, 1, 1 }, { 0, 0, 2 },
};
static int Need[N][M];

static bool kucuk_esit(const int a[M], const int b[M])
{
    for (int j = 0; j < M; j++)
        if (a[j] > b[j])
            return false;
    return true;
}

static void vektor_yaz(const int v[M])
{
    printf("(%d %d %d)", v[0], v[1], v[2]);
}

/* Work <- Available; Finish <- false
 * for i = T0 ... T4 (tekrar tekrar):
 *   if !Finish[i] and Need[i] <= Work: Work <- Work + Allocation[i]; Finish[i] <- true
 * return tum Finish[i] */
static bool guvenli(void)
{
    int Work[M];
    bool Finish[N] = { false };
    int sira[N], k = 0;
    bool ilerledi = true;

    memcpy(Work, Available, sizeof Work);
    while (ilerledi) {                      /* tekrar tekrar */
        ilerledi = false;
        for (int i = 0; i < N; i++) {
            if (!Finish[i] && kucuk_esit(Need[i], Work)) {
                for (int j = 0; j < M; j++)
                    Work[j] += Allocation[i][j];
                Finish[i] = true;
                sira[k++] = i;
                ilerledi = true;
            }
        }
    }
    printf("  sira:");
    for (int i = 0; i < k; i++)
        printf(" T%d", sira[i]);
    printf("  Work = ");
    vektor_yaz(Work);
    printf(" -> %s\n", k == N ? "GUVENLI" : "GUVENSIZ");
    return k == N;
}

/* Ti, Request isterse. Donus: 1 verildi, 0 bekler, -1 hata */
static int istek(int i, const int Request[M])
{
    printf("T%d, Request = ", i);
    vektor_yaz(Request);
    printf(" istiyor\n");
    if (!kucuk_esit(Request, Need[i])) {
        printf("  hata: Max asildi\n");
        return -1;
    }
    if (!kucuk_esit(Request, Available)) {
        printf("  T%d bekler: kaynak yok\n", i);
        return 0;
    }
    for (int j = 0; j < M; j++) {           /* varmis gibi yap */
        Available[j] -= Request[j];
        Allocation[i][j] += Request[j];
        Need[i][j] -= Request[j];
    }
    if (guvenli()) {
        printf("  istek VERILDI\n");
        return 1;
    }
    for (int j = 0; j < M; j++) {           /* geri al */
        Available[j] += Request[j];
        Allocation[i][j] -= Request[j];
        Need[i][j] += Request[j];
    }
    printf("  geri alindi, T%d bekler\n", i);
    return 0;
}

int main(void)
{
    const int r1[M] = { 0, 1, 1 };
    const int r2[M] = { 3, 3, 0 };

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++)
            Need[i][j] = Max[i][j] - Allocation[i][j];
        printf("T%d Need = ", i);
        vektor_yaz(Need[i]);
        printf("\n");
    }

    printf("Guvenlik testi, Available = (3 3 2):\n");
    guvenli();

    istek(3, r1);
    istek(3, r1);                           /* Need[3] artik (0 0 0) */
    istek(4, r2);                           /* Available (3 2 1): B yetmez */
    return 0;
}

/* Beklenen cikti:
 * T0 Need = (7 4 3)
 * T1 Need = (1 2 2)
 * T2 Need = (6 0 0)
 * T3 Need = (0 1 1)
 * T4 Need = (4 3 1)
 * Guvenlik testi, Available = (3 3 2):
 *   sira: T1 T3 T4 T0 T2  Work = (10 5 7) -> GUVENLI
 * T3, Request = (0 1 1) istiyor
 *   sira: T3 T4 T1 T2 T0  Work = (10 5 7) -> GUVENLI
 *   istek VERILDI
 * T3, Request = (0 1 1) istiyor
 *   hata: Max asildi
 * T4, Request = (3 3 0) istiyor
 *   T4 bekler: kaynak yok
 */
