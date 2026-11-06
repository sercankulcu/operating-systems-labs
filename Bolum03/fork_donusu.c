/* fork(): bir cagri, iki donus. Cocukta fork 0, ebeveynde cocugun PID'ini
 * dondurur; hata durumunda -1. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid = fork();
    if (pid < 0)        { perror("fork"); exit(1); }
    else if (pid == 0)  { /* cocuk: fork 0 dondurdu */
        printf("cocuk: fork 0 dondurdu, benim PID'im %d\n", (int)getpid());
        return 0;
    }
    else                { /* ebeveyn: pid = cocugun PID'i */
        waitpid(pid, NULL, 0);
        printf("ebeveyn: fork %d dondurdu (cocugun PID'i)\n", (int)pid);
    }
    return 0;
}

/* Beklenen cikti (PID'ler degisir; iki sayi aynidir):
 * cocuk: fork 0 dondurdu, benim PID'im 1235
 * ebeveyn: fork 1235 dondurdu (cocugun PID'i)
 */
