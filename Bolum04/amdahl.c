/* Amdahl yasasi: programin S kadari seri ise N cekirdekte hizlanma
 * 1 / (S + (1 - S) / N) olur ve 1 / S'yi asamaz. */
#include <stdio.h>

int main(void)
{
    double S = 0.1;            /* seri oran */
    int N = 1;                 /* cekirdek */
    while (N <= 16) {
        double h = 1 / (S + (1 - S) / N);
        printf("N=%2d  %.2fx\n", N, h);
        N = N * 2;
    }
    printf("sinir %.0fx\n", 1 / S);
    return 0;
}

/* Beklenen cikti:
 * N= 1  1.00x
 * N= 2  1.82x
 * N= 4  3.08x
 * N= 8  4.71x
 * N=16  6.40x
 * sinir 10x
 */
