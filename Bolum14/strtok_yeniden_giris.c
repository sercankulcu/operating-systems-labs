/* Yeniden kullanim ve yeniden girislilik: strtok durumunu static bir
 * isaretcide tuttugu icin araya giren bir cagri (kesme isleyicisi) main'in
 * kaldigi yeri bozar. strtok_r durumu cagirana birakir ve sorunu cozer.
 * "Kesme" deterministik olarak iki cagri arasinda benzetilir. */
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>

char *my_strtok(char *s, char ayr)
{
    static char *kalan;     /* herkes paylasir! */
    if (s) kalan = s;
    if (!kalan || !*kalan) return NULL;
    char *bas = kalan;
    while (*kalan && *kalan != ayr) kalan++;
    if (*kalan) *kalan++ = '\0';
    return bas;
}

int main(void)
{
    char a[] = "ali,veli", b[] = "x;y";

    printf("my_strtok:\n");
    printf("  main:  %s\n", my_strtok(a, ','));     /* a = "ali,veli" */
    printf("  kesme: %s\n", my_strtok(b, ';'));     /* b = "x;y" */
    printf("  main:  %s  (hatali, beklenen veli)\n", my_strtok(NULL, ','));

    /* libc strtok da ayni static durumu kullanir */
    char a2[] = "ali,veli", b2[] = "x;y";
    printf("strtok:\n");
    printf("  main:  %s\n", strtok(a2, ","));
    printf("  kesme: %s\n", strtok(b2, ";"));
    printf("  main:  %s  (hatali)\n", strtok(NULL, ","));

    /* yeniden girisli surum: durum cagiranda */
    char a3[] = "ali,veli", b3[] = "x;y";
    char *main_kalan, *kesme_kalan;
    printf("strtok_r:\n");
    printf("  main:  %s\n", strtok_r(a3, ",", &main_kalan));
    printf("  kesme: %s\n", strtok_r(b3, ";", &kesme_kalan));
    printf("  main:  %s  (dogru)\n", strtok_r(NULL, ",", &main_kalan));
    return 0;
}

/* Beklenen cikti:
 * my_strtok:
 *   main:  ali
 *   kesme: x
 *   main:  y  (hatali, beklenen veli)
 * strtok:
 *   main:  ali
 *   kesme: x
 *   main:  y  (hatali)
 * strtok_r:
 *   main:  ali
 *   kesme: x
 *   main:  veli  (dogru)
 */
