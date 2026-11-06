/* open, read, close adim adim: 6000 baytlik dosya 4096 baytlik tamponla okunur.
 * Ilk read 4096, ikincisi kisa okuma 1904, ucuncusu 0 (dosya sonu) dondurur.
 * notlar.txt once /tmp'de olusturulur, sonda silinir. */
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define NOTLAR "/tmp/notlar.txt"

static int hazirla(void)
{
    char tampon[6000];
    int fd = open(NOTLAR, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
        return -1;
    memset(tampon, 'n', sizeof tampon);
    ssize_t w = write(fd, tampon, sizeof tampon);
    close(fd);
    return w == (ssize_t)sizeof tampon ? 0 : -1;
}

int main(void)
{
    if (hazirla() != 0) { perror("hazirla"); return 1; }

    char buf[4096];
    int fd = open(NOTLAR, O_RDONLY);
    printf("fd = %d\n", fd);
    ssize_t n = read(fd, buf, sizeof buf);
    printf("n = %zd, konum = %lld\n", n, (long long)lseek(fd, 0, SEEK_CUR));
    n = read(fd, buf, sizeof buf);
    printf("n = %zd, konum = %lld\n", n, (long long)lseek(fd, 0, SEEK_CUR));
    n = read(fd, buf, sizeof buf);
    printf("n = %zd (dosya sonu)\n", n);
    close(fd);

    unlink(NOTLAR);
    return 0;
}

/* Beklenen cikti (0, 1, 2 aciksa en kucuk bos numara 3'tur):
 * fd = 3
 * n = 4096, konum = 4096
 * n = 1904, konum = 6000
 * n = 0 (dosya sonu)
 */
