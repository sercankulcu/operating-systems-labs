/* Degiskenler nereye yerlesir: metin, veri, BSS, heap ve yigin bolgelerindeki
 * nesnelerin adresleri yazdirilir.
 * Derlenmis programda:  size a.out    (metin/veri/BSS boyutlari)
 * Calisan surecte:      cat /proc/PID/maps */
#include <stdio.h>
#include <stdlib.h>

int sayac = 5;    /* data */
int tablo[1000];  /* bss */

int main(int argc, char *argv[])
{
    int yerel = 3;
    static int adet;
    int *p = malloc(4 * sizeof *p);
    if (p == NULL)
        return 1;
    p[0] = yerel + sayac;

    printf("metin : main   = %p\n", (void *)main);
    printf("veri  : sayac  = %p (deger %d)\n", (void *)&sayac, sayac);
    printf("bss   : tablo  = %p (tablo[0] = %d)\n", (void *)tablo, tablo[0]);
    printf("bss   : adet   = %p (deger %d)\n", (void *)&adet, adet);
    printf("heap  : *p     = %p (p[0] = %d)\n", (void *)p, p[0]);
    printf("yigin : yerel  = %p, p = %p, argc = %p\n",
           (void *)&yerel, (void *)&p, (void *)&argc);
    (void)argv;

    free(p);
    return 0;
}

/* Beklenen cikti (adresler her calistirmada degisir, ASLR):
 * metin : main   = 0x55...      en kucuk adres
 * veri  : sayac  = 0x55... (deger 5)
 * bss   : tablo  = 0x55... (tablo[0] = 0)
 * bss   : adet   = 0x55... (deger 0)
 * heap  : *p     = 0x55... (p[0] = 8)   veri/BSS'nin hemen ustunde
 * yigin : yerel  = 0x7ff..., p = 0x7ff..., argc = 0x7ff...   en buyuk adresler
 */
