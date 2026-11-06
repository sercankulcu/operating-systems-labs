/* E-kup yonlendirme (hiperkup): ileti, adresin farkli bitlerini soldan saga
 * tek tek duzelterek komsu dugumlere atlar; en kisa yol farkli bit sayisi
 * kadardir. Ornek: 4 boyutlu kupte 0110 (6) -> 1101 (13). */
#include <stdio.h>

void yonlendir(int src, int dst, int d)
{
    int u = src;
    for (int b = d - 1; b >= 0; b--) {
        int m = 1 << b;
        if ((u ^ dst) & m) {  /* bit farkli */
            u ^= m;           /* komsuya gec */
            printf("%d ", u);
        }
    }
}

int main(void)
{
    printf("6 -> 13: ");
    yonlendir(6, 13, 4);
    printf("\n0 -> 15: ");
    yonlendir(0, 15, 4);
    printf("\n");
    return 0;
}

/* Beklenen cikti:
 * 6 -> 13: 14 12 13
 * 0 -> 15: 8 12 14 15
 */
