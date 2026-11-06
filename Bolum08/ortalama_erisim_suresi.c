/* Bellek hiyerarsisinde ortalama erisim suresi: L1, L2, L3, DRAM.
 * En alttan (DRAM) baslanir; her duzey t = sure + iska * (alttaki duzeyin suresi). */
#include <stdio.h>

int main(void)
{
    /* L1, L2, L3, DRAM erisim suresi (ns) */
    double sure[4] = {1, 4, 20, 80};
    double iska[3] = {0.10, 0.40, 0.50};
    double t = sure[3];         /* DRAM */
    for (int k = 2; k >= 0; k--)
        t = sure[k] + iska[k] * t;
    printf("ortalama = %.1f ns\n", t);
    return 0;
}

/* Beklenen cikti (ara degerler: t = 80 -> 60 -> 28 -> 3.8):
 * ortalama = 3.8 ns
 */
