/* fork() ve wait(): cocuk kendi x kopyasini degistirir, ebeveyninki 10
 * kalir. wait() sayesinde cikti sirasi her calistirmada aynidir. */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    int x = 10;
    pid_t pid = fork();
    if (pid == 0) {
        x = x + 5;
        printf("cocuk: x=%d\n", x);
        return 0;
    }
    wait(NULL);
    printf("ebeveyn: x=%d\n", x);
    return 0;
}

/* Beklenen cikti:
 * cocuk: x=15
 * ebeveyn: x=10
 */
