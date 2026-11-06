/* Bitesleme bos alan arama: 10 birimlik bellekte (1 dolu, 0 bos) k ardisik bos
 * birim aranir. Bulunan alan ayrilip bitleri 1 yapilir. */
#include <stdio.h>

/* k ardisik bos (0) birim ara */
int bul(const char bit[], int n, int k)
{
    int bas = 0, uz = 0;
    for (int i = 0; i < n; i++) {
        if (bit[i] == 0) {
            if (uz == 0) bas = i;  /* yeni kosu */
            if (++uz == k) return bas;
        } else
            uz = 0;                /* kosu bozuldu */
    }
    return -1;
}

static void yaz(const char bit[], int n)
{
    printf("bit = ");
    for (int i = 0; i < n; i++)
        printf("%d", bit[i]);
    printf("\n");
}

int main(void)
{
    char bit[10] = {1, 1, 0, 0, 1, 0, 0, 0, 1, 1};
    int b;

    yaz(bit, 10);
    b = bul(bit, 10, 3);
    printf("bul = %d\n", b);
    for (int i = b; i >= 0 && i < b + 3; i++)
        bit[i] = 1;                         /* ayir */
    yaz(bit, 10);
    printf("bul(k = 2) = %d\n", bul(bit, 10, 2));
    return 0;
}

/* Beklenen cikti:
 * bit = 1100100011
 * bul = 5
 * bit = 1100111111
 * bul(k = 2) = 2
 */
