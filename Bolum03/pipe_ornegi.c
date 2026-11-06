/* pipe() + fork(): ebeveyn borunun yazma ucuna "merhaba" yazar, cocuk okuma
 * ucundan okur. Kullanilmayan uclar kapatilir ki okuyucu EOF gorebilsin. */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    int fd[2];
    char buf[32];
    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }
    pid_t pid = fork();
    if (pid == 0) {   /* cocuk: okur */
        close(fd[1]);
        ssize_t n = read(fd[0], buf, sizeof buf - 1);
        if (n < 0)
            n = 0;
        buf[n] = '\0';
        printf("cocuk aldi: %s\n", buf);
        close(fd[0]);
        return 0;
    }
    close(fd[0]);     /* ebeveyn: yazar */
    if (write(fd[1], "merhaba", 7) != 7)
        perror("write");
    close(fd[1]);
    wait(NULL);
    return 0;
}

/* Beklenen cikti:
 * cocuk aldi: merhaba
 */
