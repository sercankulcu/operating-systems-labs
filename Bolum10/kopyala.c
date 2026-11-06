/* Dosya kopyalama (copy abc xyz): 4096 baytlik tamponla read/write dongusu.
 * 10000 baytlik abc 3 turda kopyalanir (4096 + 4096 + 1808); 4 read, 3 write.
 * Dosyalar /tmp'de olusturulur, sonda silinir.
 * Sunumdaki kullanim: $ copy abc xyz */
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF 4096

static int okuma, yazma;        /* cagri sayaclari */

int kopyala(const char *src, const char *dst)
{
    char buf[BUF];
    int in  = open(src, O_RDONLY);
    int out = open(dst, O_WRONLY | O_CREAT
                        | O_TRUNC, 0644);
    ssize_t n;
    if (in < 0 || out < 0)
        return -1;
    while ((n = read(in, buf, BUF)) > 0) {
        okuma++;
        if (write(out, buf, n) != n)        /* tam n bayt (BUF degil!) */
            break;
        yazma++;
    }
    okuma++;                                /* 0 donduren son read */
    close(in);
    close(out);
    return 0;
}

int main(void)
{
    const char *abc = "/tmp/abc", *xyz = "/tmp/xyz";
    char veri[10000];
    struct stat st;

    for (size_t i = 0; i < sizeof veri; i++)
        veri[i] = (char)('a' + i % 26);
    int fd = open(abc, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0 || write(fd, veri, sizeof veri) != (ssize_t)sizeof veri) {
        perror(abc);
        return 1;
    }
    close(fd);

    if (kopyala(abc, xyz) != 0) {
        perror("kopyala");
        unlink(abc);
        return 1;
    }
    stat(xyz, &st);
    printf("kopyalanan = %lld bayt; %d read, %d write cagrisi\n",
           (long long)st.st_size, okuma, yazma);

    char geri[10000];
    fd = open(xyz, O_RDONLY);
    ssize_t n = read(fd, geri, sizeof geri);
    close(fd);
    printf("icerik %s\n", n == (ssize_t)sizeof geri &&
           memcmp(veri, geri, sizeof veri) == 0 ? "ayni" : "FARKLI");

    unlink(abc);
    unlink(xyz);
    return 0;
}

/* Beklenen cikti:
 * kopyalanan = 10000 bayt; 4 read, 3 write cagrisi
 * icerik ayni
 */
