/* Kabukta boru hatti "ls | wc -l": iki cocuk ayni boruyu paylasir; dup2 ile
 * birincinin stdout'u borunun yazma ucuna, ikincinin stdin'i okuma ucuna
 * baglanir. Programlar boruyu fark etmez. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }
    if (fork() == 0) { if (dup2(fd[1], 1) == -1) _exit(1);
                       close(fd[0]); close(fd[1]);
                       execlp("ls", "ls", NULL);
                       _exit(127); }
    if (fork() == 0) { if (dup2(fd[0], 0) == -1) _exit(1);
                       close(fd[0]); close(fd[1]);
                       execlp("wc", "wc", "-l", NULL);
                       _exit(127); }
    close(fd[0]); close(fd[1]); wait(NULL); wait(NULL);
    return 0;
}

/* Beklenen cikti: calisma dizinindeki girdi sayisi. Bos dizinde:
 * 0
 */
