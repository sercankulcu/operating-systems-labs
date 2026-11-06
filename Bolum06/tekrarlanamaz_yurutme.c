/* Tekrarlanamaz yurutme: ayni iki surec, farkli araya girme sirasiyla farkli sonuc.
 * Once n = 5 iken uc olasi sira elle (deterministik) oynatilir, sonra count++ ve
 * count-- makine komutu duzeyinde (T0..T5) araya sokulur. */
#include <stdio.h>

/* Surec 1:  repeat  n = n + 1;
 * Surec 2:  repeat  print(n); n = 0;
 * Tek bir turu, verilen sirayla calistirir: 'A' = n = n + 1, 'P' = print, 'S' = n = 0 */
static void sira_calistir(const char *ad, const char *sira)
{
    int n = 5;
    int yazilan = -1;
    for (const char *p = sira; *p != '\0'; p++) {
        if (*p == 'A')
            n = n + 1;
        else if (*p == 'P')
            yazilan = n;          /* print(n) */
        else if (*p == 'S')
            n = 0;
    }
    printf("%-28s -> %d yazar, sonunda n = %d\n", ad, yazilan, n);
}

int main(void)
{
    printf("n = 5 iken olasi siralar:\n");
    sira_calistir("n=n+1; print; n=0", "APS");
    sira_calistir("print; n=0; n=n+1", "PSA");
    sira_calistir("print; n=n+1; n=0", "PAS");   /* artis kaybolur */

    /* count++ ve count-- makinede uc komuttur; slayttaki T0..T5 sirasi */
    int count = 5;
    int reg1, reg2;
    reg1 = count;          /* T0 uretici:  reg1 = count     (5) */
    reg1 = reg1 + 1;       /* T1 uretici:  reg1 = reg1 + 1  (6) */
    reg2 = count;          /* T2 tuketici: reg2 = count     (5) */
    reg2 = reg2 - 1;       /* T3 tuketici: reg2 = reg2 - 1  (4) */
    count = reg1;          /* T4 uretici:  count = reg1     (6) */
    printf("T4 sonrasi count = %d\n", count);
    count = reg2;          /* T5 tuketici: count = reg2     (4) */
    printf("T5 sonrasi count = %d (dogrusu 5)\n", count);
    return 0;
}

/* Beklenen cikti:
 * n = 5 iken olasi siralar:
 * n=n+1; print; n=0            -> 6 yazar, sonunda n = 0
 * print; n=0; n=n+1            -> 5 yazar, sonunda n = 1
 * print; n=n+1; n=0            -> 5 yazar, sonunda n = 0
 * T4 sonrasi count = 6
 * T5 sonrasi count = 4 (dogrusu 5)
 */
