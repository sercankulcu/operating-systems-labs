/* Liu-Layland testi (hiz-monoton, RM): toplam kullanim U <= n(2^(1/n) - 1)
 * ise RM tum son tarihleri karsilar (1). U <= 1 ise test karar veremez (0),
 * U > 1 ise hicbir algoritma karsilayamaz (-1). */
#include <stdio.h>
#include <math.h>

typedef struct { int p, t; } Gorev;   /* p: periyot, t: calisma suresi */

int rm_test(Gorev g[], int n)
{
    double U = 0;
    for (int i = 0; i < n; i++)
        U += (double) g[i].t / g[i].p;
    double sinir = n * (pow(2, 1.0/n) - 1);
    printf("  U=%.4f sinir=%.4f\n", U, sinir);
    if (U <= sinir) return 1; /* yeter */
    if (U <= 1.0) return 0;   /* belirsiz */
    return -1;            /* imkansiz */
}

int main(void)
{
    Gorev g1[] = { {20, 5}, {50, 10}, {100, 30} };
    Gorev g2[] = { {50, 25}, {80, 35} };
    Gorev g3[] = { {10, 6}, {10, 5} };       /* ek ornek: U > 1 */

    printf("rm_test = %d\n", rm_test(g1, 3));
    printf("rm_test = %d\n", rm_test(g2, 2));
    printf("rm_test = %d\n", rm_test(g3, 2));
    return 0;
}

/* Beklenen cikti:
 *   U=0.7500 sinir=0.7798
 * rm_test = 1
 *   U=0.9375 sinir=0.8284
 * rm_test = 0
 *   U=1.1000 sinir=0.8284
 * rm_test = -1
 */
