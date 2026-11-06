/* Kabuk "ls | wc -l" boru hattini nasil kurar: pipe, iki fork, dup2 ile
 * stdout/stdin yonlendirme, execlp. Kabuk iki ucu da kapatmazsa wc EOF
 * gormez. Ornek, icinde 7 dosya olan gecici bir dizinde calisir.
 * Kabuktaki karsiligi:  $ ls | wc -l */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

static char dizin[] = "/tmp/boru_XXXXXX";
#define NDOSYA 7

int main(void)
{
    char ad[64];
    if (!mkdtemp(dizin)) { perror("mkdtemp"); return 1; }
    for (int i = 0; i < NDOSYA; i++) {
        snprintf(ad, sizeof ad, "%s/dosya%d", dizin, i);
        FILE *f = fopen(ad, "w");
        if (f) fclose(f);
    }
    if (chdir(dizin) != 0) { perror("chdir"); return 1; }

    int fd[2];
    if (pipe(fd) != 0) { perror("pipe"); return 1; }   /* 3: okuma, 4: yazma */
    printf("pipe: fd[0] = %d, fd[1] = %d\n", fd[0], fd[1]);
    fflush(stdout);                    /* cocuklar tamponu kopyalamasin */

    if (fork() == 0) {    /* cocuk 1 */
        dup2(fd[1], 1);
        close(fd[0]); close(fd[1]);
        execlp("ls", "ls", (char *)NULL);
        _exit(127);
    }
    if (fork() == 0) {    /* cocuk 2 */
        dup2(fd[0], 0);
        close(fd[0]); close(fd[1]);
        execlp("wc", "wc", "-l", (char *)NULL);
        _exit(127);
    }
    /* kabuk: iki ucu da kapat */
    close(fd[0]); close(fd[1]);
    wait(NULL); wait(NULL);

    /* temizlik */
    for (int i = 0; i < NDOSYA; i++) {
        snprintf(ad, sizeof ad, "%s/dosya%d", dizin, i);
        unlink(ad);
    }
    if (chdir("/") != 0)
        perror("chdir");
    rmdir(dizin);
    return 0;
}

/* Beklenen cikti:
 * pipe: fd[0] = 3, fd[1] = 4
 * 7
 * (fd numaralari, kalitimla acik baska tanimlayici yoksa 3 ve 4 olur)
 */
