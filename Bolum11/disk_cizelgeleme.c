/* Disk kolu cizelgeleme: SSTF her adimda kafaya en yakin bekleyen istegi
 * secer; LOOK once yukari, sonra asagi tarar. Kafa silindir 50'de, kuyruk
 * (gelis sirasi) 82, 170, 43, 140, 24, 16, 190. */
#include <stdio.h>
#include <stdlib.h>

#define NMAX 16

int bitti[NMAX];             /* SSTF: hizmet verilen istekler */

int sstf(int bas, int q[], int n)
{
    int top = 0;
    for (int k = 0; k < n; k++) {
        int en = -1;
        for (int i = 0; i < n; i++)
            if (!bitti[i] && (en < 0 ||
                abs(q[i]-bas) < abs(q[en]-bas)))
                en = i;            /* en yakin */
        top += abs(q[en] - bas);
        bas = q[en];
        bitti[en] = 1;
        printf("%d ", bas);
    }
    printf("\n");
    return top;
}

/* s[] sirali; once yukari, sonra asagi */
int look(int bas, int s[], int n)
{
    int i = 0, top = 0;
    while (i < n && s[i] < bas) i++;
    for (int j = i; j < n; j++) {
        top += s[j] - bas; bas = s[j];
        printf("%d ", bas);
    }
    for (int j = i - 1; j >= 0; j--) {
        top += bas - s[j]; bas = s[j];
        printf("%d ", bas);
    }
    printf("\n");
    return top;
}

static int karsilastir(const void *a, const void *b)
{
    return *(const int *)a - *(const int *)b;
}

int main(void)
{
    int q[] = { 82, 170, 43, 140, 24, 16, 190 };
    int n = (int)(sizeof q / sizeof q[0]);
    int s[NMAX];

    printf("SSTF: ");
    int t1 = sstf(50, q, n);
    printf("SSTF toplam = %d silindir\n", t1);

    for (int k = 0; k < n; k++)
        s[k] = q[k];
    qsort(s, (size_t)n, sizeof s[0], karsilastir);
    printf("LOOK: ");
    int t2 = look(50, s, n);
    printf("LOOK toplam = %d silindir\n", t2);
    return 0;
}

/* Beklenen cikti:
 * SSTF: 43 24 16 82 140 170 190
 * SSTF toplam = 208 silindir
 * LOOK: 82 140 170 190 43 24 16
 * LOOK toplam = 314 silindir
 */
