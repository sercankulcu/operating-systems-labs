/* write() sistem cagrisi: 1 numarali tanimlayiciya (stdout) 17 bayt yazar.
 * "Dunya"daki u harfi UTF-8'de iki bayttir (\xc3\xbc), bu yuzden 17 bayt. */
#include <unistd.h>      // write()
#include <string.h>      // strlen()
#include <stdio.h>       // printf()

int main(void)
{
    const char *msg = "Merhaba, D\xc3\xbcnya!\n";
    size_t len = strlen(msg);        // 17 bayt
    ssize_t n = write(1, msg, len);  // 1: stdout
    if (n == -1)
        return 1;                    // hata
    printf("len=%zu n=%zd\n", len, n);
    return 0;
}

/* Beklenen cikti:
 * Merhaba, Dunya!      (u yerine UTF-8 u-umlaut)
 * len=17 n=17
 */
