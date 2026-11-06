/* FAT zinciri: bellekteki FAT'te her girdi dosyanin sonraki blogunu gosterir.
 * blok_bul, bayt ofsetinin hangi disk blogunda oldugunu zinciri izleyerek bulur.
 * A dosyasi: 4 -> 7 -> 2 -> 10 -> 12, B dosyasi: 6 -> 3 -> 11 -> 14. BLOK = 1024. */
#include <stdio.h>

#define BLOK 1024
#define SON  (-1)               /* zincir sonu */

static int fat[16] = {         /* belirtilmeyen girdiler 0: bos blok */
    [2] = 10, [3] = 11, [4] = 7, [6] = 3, [7] = 2,
    [10] = 12, [11] = 14, [12] = SON, [14] = SON,
};

int blok_bul(int ilk, long ofset)
{
    int b = ilk;
    long n = ofset / BLOK; /* blok sirasi */
    while (n > 0 && b != SON) {
        b = fat[b];          /* zinciri izle */
        n--;
    }
    return b;          /* SON: dosya disi */
}

static void dene(int ilk, long ofset)
{
    int b = blok_bul(ilk, ofset);

    if (b == SON)
        printf("blok_bul(%d, %ld) = SON (dosya disi)\n", ilk, ofset);
    else
        printf("blok_bul(%d, %ld) = %d, blok icinde %ld. bayt\n", ilk, ofset, b,
               ofset % BLOK);
}

int main(void)
{
    dene(4, 3500);
    dene(6, 5000);
    return 0;
}

/* Beklenen cikti:
 * blok_bul(4, 3500) = 10, blok icinde 428. bayt
 * blok_bul(6, 5000) = SON (dosya disi)
 */
