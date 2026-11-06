/* link ve unlink: bir dosyaya ikinci ad verilir, sonra iki ad da silinir.
 * Dosya acik bir tanimlayiciyla tutuldugu icin hala okunabilir; i-node
 * ancak close ile son tanimlayici kapaninca serbest kalir.
 * /usr/jim ve /usr/ast yerine /tmp altinda gecici dizinler kullanilir. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

static char kok[] = "/tmp/os02_linkXXXXXX";
static char jim[64], ast[64], memo[96], note[96];

static void bas_sayisi(int fd, const char *olay)
{
    struct stat st;
    if (fstat(fd, &st) == 0)
        printf("%-22s bag sayisi=%lu boyut=%lld\n", olay,
               (unsigned long)st.st_nlink, (long long)st.st_size);
}

/* 85 baytlik /usr/jim/memo dosyasini olusturur. */
static int hazirla(void)
{
    char icerik[85];
    if (mkdtemp(kok) == NULL)
        return -1;
    snprintf(jim, sizeof jim, "%s/jim", kok);
    snprintf(ast, sizeof ast, "%s/ast", kok);
    snprintf(memo, sizeof memo, "%s/memo", jim);
    snprintf(note, sizeof note, "%s/note", ast);
    if (mkdir(jim, 0755) == -1 || mkdir(ast, 0755) == -1)
        return -1;
    memset(icerik, 'm', sizeof icerik);
    int fd = creat(memo, 0644);
    if (fd == -1)
        return -1;
    if (write(fd, icerik, sizeof icerik) != (ssize_t)sizeof icerik) {
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

int main(void)
{
    char buf[100];
    ssize_t n;

    if (hazirla() == -1) {
        perror("hazirla");
        return 1;
    }

    int fd = open(memo, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    printf("fd=%d\n", fd);
    bas_sayisi(fd, "open(memo)");
    if (link(memo, note) == -1)
        perror("link");
    bas_sayisi(fd, "link(memo, note)");
    unlink(memo);
    bas_sayisi(fd, "unlink(memo)");
    unlink(note);
    bas_sayisi(fd, "unlink(note)");
    n = read(fd, buf, 100);    /* hala okunur */
    printf("n=%zd\n", n);
    close(fd);

    rmdir(jim);
    rmdir(ast);
    rmdir(kok);
    return 0;
}

/* Beklenen cikti:
 * fd=3
 * open(memo)             bag sayisi=1 boyut=85
 * link(memo, note)       bag sayisi=2 boyut=85
 * unlink(memo)           bag sayisi=1 boyut=85
 * unlink(note)           bag sayisi=0 boyut=85
 * n=85
 */
