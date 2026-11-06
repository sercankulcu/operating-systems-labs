/* Neden fork ve exec ayri? Cocuk fork ile exec arasinda stdout'u kapatip bir
 * dosya acar; open en kucuk bos numarayi (1) verir ve exec edilen wc
 * habersizce dosyaya yazar. Kabuk "wc -l notlar.txt > sayim.txt"i boyle yapar.
 * Dosyalar calisma dizini yerine /tmp altinda olusturulur ve sonda silinir. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NOTLAR "/tmp/os03_notlar.txt"
#define SAYIM  "/tmp/os03_sayim.txt"

/* 3 satirlik ornek notlar.txt */
static int notlari_hazirla(void)
{
    const char *metin = "satir 1\nsatir 2\nsatir 3\n";
    int fd = open(NOTLAR, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
        return -1;
    ssize_t n = write(fd, metin, strlen(metin));
    close(fd);
    return n == (ssize_t)strlen(metin) ? 0 : -1;
}

int main(void)
{
    if (notlari_hazirla() == -1) {
        perror("notlar");
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {    /* cocuk */
        close(1);        /* stdout bosalir */
        int fd = open(SAYIM,
                 O_WRONLY | O_CREAT | O_TRUNC, 0644);
        /* en kucuk bos numara: fd == 1 */
        if (fd != 1)
            _exit(2);
        execlp("wc", "wc", "-l", NOTLAR, NULL);
        perror("execlp");
        exit(1);
    }
    waitpid(pid, NULL, 0);
    printf("bitti\n");

    /* Dosyaya ne yazildigini gosterir. */
    char buf[128];
    int fd = open(SAYIM, O_RDONLY);
    if (fd != -1) {
        ssize_t n = read(fd, buf, sizeof buf - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("sayim.txt: %s", buf);
        }
        close(fd);
    }
    unlink(SAYIM);
    unlink(NOTLAR);
    return 0;
}

/* Beklenen cikti (ekranda yalnizca "bitti"; ikinci satir dosyanin icerigi):
 * bitti
 * sayim.txt: 3 /tmp/os03_notlar.txt
 */
