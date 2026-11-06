/* TOCTOU yaris durumu: access() ile kontrol edilen ad, open() ile
 * kullanilana kadar baska bir dosyaya giden sembolik baga cevrilebilir.
 * Saldirganin araya girmesi deterministik olarak benzetilir; hedef olarak
 * /etc/passwd yerine /tmp altindaki "gizli" dosya kullanilir. */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

static char dizin[] = "/tmp/toctou_XXXXXX";
static char path[64], gizli[64];

static void dosya_yaz(const char *ad, const char *icerik)
{
    int fd = open(ad, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { perror(ad); return; }
    if (write(fd, icerik, strlen(icerik)) < 0)
        perror("write");
    close(fd);
}

static void gizli_goster(void)
{
    char b[64];
    int fd = open(gizli, O_RDONLY);
    ssize_t n = fd < 0 ? -1 : read(fd, b, sizeof b - 1);
    if (fd >= 0)
        close(fd);
    if (n < 0) n = 0;
    b[n] = '\0';
    printf("  gizli dosyanin icerigi: \"%s\"\n", b);
}

static void saldirgan(void)
{
    /* kontrol ile kullanim arasindaki pencerede ad degistirilir */
    unlink(path);
    if (symlink(gizli, path) != 0)
        perror("symlink");
}

int main(void)
{
    const char *data = "SALDIRGAN";
    size_t n = strlen(data);
    int fd;

    if (!mkdtemp(dizin)) { perror("mkdtemp"); return 1; }
    snprintf(path, sizeof path, "%s/f", dizin);
    snprintf(gizli, sizeof gizli, "%s/gizli", dizin);

    /* --- hatali: kontrol ve kullanim ayri --- */
    dosya_yaz(gizli, "root:x:0:0");
    dosya_yaz(path, "");
    printf("hatali surum:\n");
    /* TOCTOU: setuid root yardimci */
    if (access(path, W_OK) == 0) {   /* KONTROL */
        printf("  access: yazilabilir\n");
        saldirgan();
        fd = open(path, O_WRONLY);     /* KULLANIM */
        if (write(fd, data, n) < 0)
            perror("write");
        close(fd);
    }
    gizli_goster();

    /* --- duzeltme: ad bir kez cozulur, sembolik bag izlenmez --- */
    dosya_yaz(gizli, "root:x:0:0");
    unlink(path);
    dosya_yaz(path, "");
    printf("duzeltilmis surum:\n");
    if (access(path, W_OK) == 0) {
        printf("  access: yazilabilir\n");
        saldirgan();
        fd = open(path, O_WRONLY | O_NOFOLLOW);
        if (fd < 0) {
            printf("  open(O_NOFOLLOW): %s\n", errno == ELOOP ? "ELOOP, reddedildi" : strerror(errno));
        } else {
            if (write(fd, data, n) < 0)
                perror("write");
            close(fd);
        }
    }
    gizli_goster();

    unlink(path);
    unlink(gizli);
    rmdir(dizin);
    return 0;
}

/* Beklenen cikti:
 * hatali surum:
 *   access: yazilabilir
 *   gizli dosyanin icerigi: "SALDIRGAN0"
 * duzeltilmis surum:
 *   access: yazilabilir
 *   open(O_NOFOLLOW): ELOOP, reddedildi
 *   gizli dosyanin icerigi: "root:x:0:0"
 * ("root:x:0:0" 10 bayt; 9 baytlik "SALDIRGAN" ustune yazilinca son "0" kalir)
 */
