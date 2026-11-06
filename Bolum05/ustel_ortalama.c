/* Ustel ortalama ile sonraki CPU patlamasini tahmin etmek (SJF icin):
 * tau(n+1) = a * t(n) + (1 - a) * tau(n). */
#include <stdio.h>

int main(void)
{
    double tau = 10.0;      /* tau0 */
    const double a = 0.5;  /* alfa */
    int t[] = { 6, 4, 6, 4, 13, 13, 13 };
    for (int n = 0; n < 7; n++) {
        printf("%.1f %d\n", tau, t[n]);
        tau = a * t[n] + (1 - a) * tau;
    }
    printf("sonraki: %.1f\n", tau);
    return 0;
}

/* Beklenen cikti (tahmin, gercek):
 * 10.0 6
 * 8.0 4
 * 6.0 6
 * 6.0 4
 * 5.0 13
 * 9.0 13
 * 11.0 13
 * sonraki: 12.0
 */
