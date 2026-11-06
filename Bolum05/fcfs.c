/* FCFS (ilk gelen ilk hizmet alir) bekleme suresi: surecler gelis sirasiyla
 * calisir; bekleme = CPU'nun bosaldigi an - gelis. */
#include <stdio.h>

typedef struct {
    const char *ad;
    int gelis, sure;
} Surec;

/* p[] gelis sirasina gore sirali */
void fcfs(Surec p[], int n)
{
    int t = 0, top = 0;
    for (int i = 0; i < n; i++) {
        if (t < p[i].gelis) t = p[i].gelis;
        int bekleme = t - p[i].gelis;
        t += p[i].sure;          /* bitis */
        top += bekleme;
        printf("%s: %d\n", p[i].ad, bekleme);
    }
    printf("ort = %.2f\n", (double) top / n);
}

int main(void)
{
    Surec p[] = {
        {"P1", 0, 8},
        {"P2", 1, 4},
        {"P3", 2, 9},
        {"P4", 3, 5},
    };
    fcfs(p, 4);
    return 0;
}

/* Beklenen cikti:
 * P1: 0
 * P2: 7
 * P3: 10
 * P4: 18
 * ort = 8.75
 */
