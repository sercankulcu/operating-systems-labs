/* Coklu programlama modeli: her surec zamaninin p kadarini G/C bekleyerek
 * geciriyorsa, bellekte n surec varken CPU kullanimi 1 - p^n olur. */
#include <stdio.h>

int main(void)
{
    double p = 0.8;      /* G/C orani */
    double pn = 1.0;
    for (int n = 1; n <= 5; n++) {
        pn = pn * p;       /* hepsi bekler */
        double kul = 1 - pn;
        printf("n=%d %%%.0f\n", n, kul * 100);
    }
    return 0;
}

/* Beklenen cikti:
 * n=1 %20
 * n=2 %36
 * n=3 %49
 * n=4 %59
 * n=5 %67
 */
