/* fork + exec + waitpid: cocuk execlp ile kendini "ls -l" programiyla
 * degistirir, ebeveyn waitpid ile bekleyip cikis durumunu yazar. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); exit(1); }
    if (pid == 0) {
        execlp("ls", "ls", "-l", NULL);
        perror("execlp");  /* hata */
        exit(1);
    }
    int st;
    if (waitpid(pid, &st, 0) < 0) { perror("waitpid"); exit(1); }
    if (WIFEXITED(st)) printf("bitti: %d\n", WEXITSTATUS(st));
    return 0;
}

/* Beklenen cikti (ls -l ciktisi calisma dizinine baglidir; bos dizinde):
 * total 0
 * bitti: 0
 */
