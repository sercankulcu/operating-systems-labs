/* read/write ile dosya kopyalama: 10 000 baytlik "kaynak" dosyasi 4096
 * baytlik tamponla "hedef"e kopyalanir (4096 + 4096 + 1808). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define KAYNAK "/tmp/os02_kaynak"
#define HEDEF  "/tmp/os02_hedef"

/* 10 000 baytlik ornek kaynak dosyayi hazirlar. */
static int kaynak_hazirla(void)
{
    char blok[1000];
    memset(blok, 'x', sizeof blok);
    int fd = creat(KAYNAK, 0644);
    if (fd == -1)
        return -1;
    for (int i = 0; i < 10; i++) {
        if (write(fd, blok, sizeof blok) != (ssize_t)sizeof blok) {
            close(fd);
            return -1;
        }
    }
    close(fd);
    return 0;
}

int main(void)
{
    if (kaynak_hazirla() == -1) {
        perror("kaynak");
        return 1;
    }

    char buf[4096];
    int in = open(KAYNAK, O_RDONLY);
    int out = creat(HEDEF, 0644);
    if (in == -1 || out == -1) {
        perror("open");
        unlink(KAYNAK);
        return 1;
    }
    printf("in=%d out=%d\n", in, out);
    ssize_t n;  long toplam = 0;
    while ((n = read(in, buf, 4096)) > 0) {
        if (write(out, buf, n) != n) {
            perror("write");
            break;
        }
        toplam += n;
        printf("n=%zd toplam=%ld\n", n, toplam);
    }
    close(in);  close(out);
    printf("%ld bayt\n", toplam);

    unlink(KAYNAK);
    unlink(HEDEF);
    return 0;
}

/* Beklenen cikti (stdin/stdout/stderr acik oldugundan ilk bos fd 3):
 * in=3 out=4
 * n=4096 toplam=4096
 * n=4096 toplam=8192
 * n=1808 toplam=10000
 * 10000 bayt
 */
