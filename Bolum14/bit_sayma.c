/* Uzay-zaman odunlesimi: bir baytin 1 bitlerini saymanin dort yolu.
 * (a) dongu, (b) makro, (c) 256 baytlik tablo, (d) 16 baytlik yarim bayt
 * tablosu. Sunumda (a), (b), (c) ayni bit_count adini kullanir; burada
 * makrolar sirayla tanimlanip #undef ile kaldirilir. */
#include <stdio.h>

/* (a) dongu */
#define BYTE_SIZE 8
int bit_count(int byte)
{
    int i, count = 0;
    for (i = 0; i < BYTE_SIZE; i++)
        if ((byte >> i) & 1)
            count++;
    return count;
}

/* (b) makro: dongu yok */
#define bit_count(b) ((b&1) + ((b>>1)&1) + \
    ((b>>2)&1) + ((b>>3)&1) + ((b>>4)&1) + \
    ((b>>5)&1) + ((b>>6)&1) + ((b>>7)&1))
static int makro_ile(int x) { return bit_count(x); }
#undef bit_count

/* (c) tablo: 256 bayt */
char bits[256] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
};
#define bit_count(b) (int) bits[b]
static int tablo_ile(int x) { return bit_count(x); }
#undef bit_count

/* (d) yarim bayt tablosu */
char yarim[16];          /* 16 bayt */
void tablo_kur(void)
{
    yarim[0] = 0;
    for (int i = 1; i < 16; i++)
        yarim[i] = (i & 1) + yarim[i >> 1];
}

int bit_say(unsigned char b)
{
    return yarim[b & 0x0F] + yarim[b >> 4];
}

int main(void)
{
    tablo_kur();
    printf("yarim[] =");
    for (int i = 0; i < 16; i++)
        printf(" %d", yarim[i]);
    printf("\n");

    int ornek[] = { 0xB7, 0x5C };
    for (int k = 0; k < 2; k++) {
        int b = ornek[k];
        printf("0x%02X: dongu %d, makro %d, tablo %d\n",
               b, bit_count(b), makro_ile(b), tablo_ile(b));
        printf("bit_say(0x%02X) = %d\n", b, bit_say((unsigned char)b));
    }
    return 0;
}

/* Beklenen cikti:
 * yarim[] = 0 1 1 2 1 2 2 3 1 2 2 3 2 3 3 4
 * 0xB7: dongu 6, makro 6, tablo 6
 * bit_say(0xB7) = 6
 * 0x5C: dongu 4, makro 4, tablo 4
 * bit_say(0x5C) = 4
 */
