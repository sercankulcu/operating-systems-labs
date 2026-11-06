/* Dogrudan eslemeli onbellek: 64 B satir, 4 kume. Her blok tek bir kumeye gider;
 * ayni kumeye dusen bloklar birbirini kovar (cakisma iskasi).
 * Etiket yerine blok numarasinin tamami saklanir. */
#include <stdio.h>

int main(void)
{
    int adres[6] = {0, 8, 64, 256, 0, 72};
    int etiket[4] = {-1, -1, -1, -1};
    int isabet = 0, iska = 0;

    for (int i = 0; i < 6; i++) {
        int blok = adres[i] / 64;
        int kume = blok % 4;
        int onceki = isabet;
        if (etiket[kume] == blok) isabet++;
        else { etiket[kume] = blok; iska++; }
        printf("adres %3d: blok %d, kume %d -> %s\n", adres[i], blok, kume,
               isabet > onceki ? "isabet" : "iska");
    }
    printf("etiket = [%d, %d, %d, %d]\n", etiket[0], etiket[1], etiket[2], etiket[3]);
    printf("isabet = %d, iska = %d\n", isabet, iska);
    return 0;
}

/* Beklenen cikti:
 * adres   0: blok 0, kume 0 -> iska
 * adres   8: blok 0, kume 0 -> isabet
 * adres  64: blok 1, kume 1 -> iska
 * adres 256: blok 4, kume 0 -> iska
 * adres   0: blok 0, kume 0 -> iska
 * adres  72: blok 1, kume 1 -> isabet
 * etiket = [0, 1, -1, -1]
 * isabet = 2, iska = 4
 */
