/* En az ayricalik ve setuid: passwd gibi bir setuid root program ayricaligi
 * yalniz gereken is icin kullanir, sonra setresuid ile gercek, etkin ve
 * sakli kimligin ucunu de kullaniciya indirir; geri donus yoktur.
 * Program setuid root kurulmadan calistigi icin ilk acilan "golge" dosya
 * /tmp altinda bir benzetimdir; son open gercek /etc/shadow'u dener. */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

static char golge_yolu[] = "/tmp/golge_XXXXXX";   /* /etc/shadow benzetimi */

static void update_own_entry(int fd, uid_t me)
{
    char satir[64];
    int n = snprintf(satir, sizeof satir, "uid %u: yeni parola ozeti\n", (unsigned)me);
    if (write(fd, satir, (size_t)n) != n)
        perror("write");
}

static void kimlikleri_yaz(const char *an)
{
    uid_t r, e, s;
    if (getresuid(&r, &e, &s) != 0) {
        perror("getresuid");
        return;
    }
    printf("%-22s ruid=%u euid=%u suid=%u\n", an, (unsigned)r, (unsigned)e, (unsigned)s);
}

int main(void)
{
    /* /usr/bin/passwd  -rwsr-xr-x root */
    int t = mkstemp(golge_yolu);
    if (t < 0) { perror("mkstemp"); return 1; }
    close(t);

    uid_t me = getuid();
    kimlikleri_yaz("baslangic:");
    int fd = open(golge_yolu, O_RDWR);            /* gercekte /etc/shadow */
    update_own_entry(fd, me);
    close(fd);
    if (setresuid(me, me, me) != 0)
        perror("setresuid");
    kimlikleri_yaz("setresuid sonrasi:");
    /* ... kalan is ... */
    int fd2 = open("/etc/shadow", O_RDWR);
    if (fd2 < 0) {
        printf("open(\"/etc/shadow\"): %s\n", strerror(errno));
    } else {
        printf("open(\"/etc/shadow\") basarili: program root olarak calisiyor\n");
        close(fd2);                                /* hicbir sey yazilmaz */
    }

    unlink(golge_yolu);
    return 0;
}

/* Beklenen cikti (sira disi kullanici, ornegin uid 1000):
 * baslangic:             ruid=1000 euid=1000 suid=1000
 * setresuid sonrasi:     ruid=1000 euid=1000 suid=1000
 * open("/etc/shadow"): Permission denied
 * Gercek setuid root kurulumda (chown root, chmod u+s) baslangicta euid=0
 * ve suid=0 olur; setresuid'den sonra ucu de 1000'e iner. Uid degeri ve
 * root olarak calistirmada son satir makineye gore degisir. */
