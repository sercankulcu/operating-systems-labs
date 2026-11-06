/* Program yapisi ve yerellik: 128 x 128 int dizi, her satir bir sayfa (128 sozcuk),
 * surece 64 cerceve (LRU). Sutun sutun dolasma her erisimde hata verir,
 * satir satir dolasma satir basina bir hata. Sayfa hatalari benzetimle sayilir. */
#include <stdio.h>

#define CERCEVE 64

int data[128][128];   /* satir = 1 sayfa */

static int cer[CERCEVE];        /* cercevedeki sayfa, -1 bos */
static long son_kullanim[CERCEVE];
static long saat;
static long hata;

static void sifirla(void)
{
    for (int k = 0; k < CERCEVE; k++) {
        cer[k] = -1;
        son_kullanim[k] = 0;
    }
    saat = 0;
    hata = 0;
}

/* data[i][...]'ye erisim: sayfa i bellekte mi? degilse LRU kurbanin yerine yukle */
static void erisim(int sayfa)
{
    int kurban = 0;

    saat++;
    for (int k = 0; k < CERCEVE; k++) {
        if (cer[k] == sayfa) {
            son_kullanim[k] = saat;
            return;
        }
        if (son_kullanim[k] < son_kullanim[kurban])
            kurban = k;
    }
    hata++;
    cer[kurban] = sayfa;
    son_kullanim[kurban] = saat;
}

int main(void)
{
    sifirla();
    /* A: sutun sutun */
    for (int j = 0; j < 128; j++)
        for (int i = 0; i < 128; i++) {
            erisim(i);
            data[i][j] = 0;
        }
    printf("A (sutun sutun): %ld hata\n", hata);

    sifirla();
    /* B: satir satir */
    for (int i = 0; i < 128; i++)
        for (int j = 0; j < 128; j++) {
            erisim(i);
            data[i][j] = 0;
        }
    printf("B (satir satir): %ld hata\n", hata);
    return 0;
}

/* Beklenen cikti:
 * A (sutun sutun): 16384 hata
 * B (satir satir): 128 hata
 */
