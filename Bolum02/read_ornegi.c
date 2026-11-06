/* read() sistem cagrisi: 0 numarali tanimlayicidan (stdin) okur, okunan n
 * bayti aynen stdout'a yazar. Klavye yerine stdin'e bagli bir boru
 * kullanilir; kullanicinin "selam" yazip Enter'a basmasi taklit edilir. */
#include <unistd.h>
#include <stdio.h>

/* "Kullanici" satirini boruya yazar ve boruyu 0 numarali tanimlayici yapar. */
static int klavyeyi_taklit_et(const char *satir, size_t uzunluk)
{
    int p[2];
    if (pipe(p) == -1)
        return -1;
    if (write(p[1], satir, uzunluk) != (ssize_t)uzunluk)
        return -1;
    close(p[1]);
    if (dup2(p[0], 0) == -1)
        return -1;
    close(p[0]);
    return 0;
}

int main(void)
{
    if (klavyeyi_taklit_et("selam\n", 6) == -1) {
        perror("boru");
        return 1;
    }

    char buf[100];
    ssize_t n = read(0, buf, sizeof(buf));
    if (n == -1) {
        perror("okuma hatasi");
        return 1;
    }
    if (write(1, buf, n) != n)    // tam n bayt
        return 1;
    printf("n=%zd\n", n);
    return 0;
}

/* Beklenen cikti:
 * selam
 * n=6
 */
