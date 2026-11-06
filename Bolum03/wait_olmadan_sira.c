/* wait() olmadan cikti sirasi belirsizdir: fork sonrasi hangi surecin once
 * calisacagini cizelgeleyici belirler. Ebeveyn, yazdirdiktan sonra cocugu
 * yine de bekler (zombi kalmasin diye); sira bundan etkilenmez. */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid = fork();
    if (pid == 0) printf("cocuk\n");
    else          printf("ebeveyn\n");

    if (pid > 0)
        wait(NULL);         /* yazdirmadan SONRA: siraya karismaz */
    return 0;
}

/* Beklenen cikti (iki satirin sirasi calistirmadan calistirmaya degisebilir):
 * ebeveyn
 * cocuk
 */
