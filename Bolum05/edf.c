/* EDF (en erken son tarih once): her milisaniyede, hazir isler arasindan son
 * tarihi en yakin olan calisir. A(P=3,C=1) B(P=4,C=1) C(P=6,C=2),
 * U = 1/3 + 1/4 + 2/6 = 0.917; 12 ms'lik ortak periyot izlenir. */
#include <stdio.h>

int main(void)
{
    const char ad[] = {'A', 'B', 'C'};
    int P[] = {3, 4, 6};
    int C[] = {1, 1, 2};
    int n = 3;
    int kalan[3] = {0, 0, 0};
    int teslim[3] = {0, 0, 0};
    int kacan = 0;

    /* A(P=3,C=1) B(P=4,C=1) C(P=6,C=2) */
    for (int t = 0; t < 12; t++) {
        for (int i = 0; i < n; i++)
            if (t % P[i] == 0) {      /* yeni is */
                if (kalan[i] > 0)
                    kacan++;          /* onceki is son tarihini kacirdi */
                kalan[i] = C[i];
                teslim[i] = t + P[i];
            }
        int s = -1;
        for (int i = 0; i < n; i++)
            if (kalan[i] > 0 &&
                (s < 0 || teslim[i] < teslim[s]))
                s = i;
        if (s >= 0) kalan[s]--;     /* 1 ms */
        printf("t=%2d: %c\n", t, s >= 0 ? ad[s] : '-');
    }
    printf("kacirilan son tarih: %d\n", kacan);
    return 0;
}

/* Beklenen cikti:
 * t= 0: A
 * t= 1: B
 * t= 2: C
 * t= 3: A
 * t= 4: C
 * t= 5: B
 * t= 6: A
 * t= 7: C
 * t= 8: B
 * t= 9: A
 * t=10: C
 * t=11: -
 * kacirilan son tarih: 0
 */
