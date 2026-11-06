/* Biteslemle blok ayirma: 16 blokluk diskte (1 dolu, 0 bos) arama ipucundan
 * (dosyanin son blogunun arkasi) baslar, dairesel ilerler. Dosya blok 3'te
 * bitiyor ve 3 blok daha istiyor: blok_ayir(4), blok_ayir(7), blok_ayir(8). */
#include <stdio.h>

#define N 16

static int bitesl[N] = {1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1};

int blok_ayir(int ipucu)
{
    for (int k = 0; k < N; k++) {
        int i = (ipucu + k) % N;
        if (bitesl[i] == 0) {  /* bos mu? */
            bitesl[i] = 1;
            return i;
        }
    }
    return -1;               /* disk dolu */
}

int main(void)
{
    int son = 3;            /* dosyanin son blogu */

    printf("ayrilan bloklar:");
    for (int j = 0; j < 3; j++) {
        son = blok_ayir(son + 1);
        printf(" %d", son);
    }
    printf("\nbitesl = ");
    for (int i = 0; i < N; i++)
        printf("%d", bitesl[i]);
    printf("\n");
    return 0;
}

/* Beklenen cikti:
 * ayrilan bloklar: 6 7 11
 * bitesl = 1101111111110111
 */
