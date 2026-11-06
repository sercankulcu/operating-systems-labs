/* En iyi uyan (best fit): istege yeten en kucuk delik secilir ve ondan ayrilir.
 * Delikler: 120, 450, 230, 300, 180 K; istekler 210 K ve 160 K. */
#include <stdio.h>

int en_iyi(int delik[], int n, int istek)
{
    int en = -1;
    for (int i = 0; i < n; i++)
        if (delik[i] >= istek &&
            (en == -1 || delik[i] < delik[en]))
            en = i;
    if (en != -1) delik[en] -= istek;
    return en;
}

static void yaz(const int delik[], int n)
{
    printf("  delik = [");
    for (int i = 0; i < n; i++)
        printf("%d%s", delik[i], i + 1 < n ? " " : "]\n");
}

int main(void)
{
    int delik[5] = {120, 450, 230, 300, 180};      /* K */

    printf("en_iyi(210) = %d\n", en_iyi(delik, 5, 210));
    yaz(delik, 5);
    printf("en_iyi(160) = %d\n", en_iyi(delik, 5, 160));
    yaz(delik, 5);
    return 0;
}

/* Beklenen cikti:
 * en_iyi(210) = 2
 *   delik = [120 450 20 300 180]
 * en_iyi(160) = 4
 *   delik = [120 450 20 300 20]
 */
