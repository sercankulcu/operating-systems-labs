/* Basit bir kabuk: istem yaz, komutu oku, fork ile cocuk olustur, cocukta
 * execve ile programi calistir, ebeveynde waitpid ile bekle. Komutlar
 * klavye yerine sabit bir listeden okunur; liste bitince dongu sona erer. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define TRUE 1

static char dizin[] = "/tmp/os02_kabukXXXXXX";
static char yol1[128], yol2[128];

/* Kullanicinin yazacagi komutlarin yerine gecen betik. */
static const char *betik[][3] = {
    {"/bin/ls", "ls", dizin},
    {"/bin/echo", "echo", "merhaba"},
};
static int sira = 0;

static void type_prompt(void)
{
    printf("$ ");
}

/* Sonraki komutu okur; komut kalmadiysa 0 dondurur. */
static int read_command(char *command, char *parameters[])
{
    if (sira >= (int)(sizeof betik / sizeof betik[0])) {
        printf("(girdi sonu)\n");
        return 0;
    }
    strcpy(command, betik[sira][0]);
    parameters[0] = (char *)betik[sira][1];
    parameters[1] = (char *)betik[sira][2];
    parameters[2] = NULL;
    printf("%s %s\n", parameters[0], parameters[1]);
    sira++;
    return 1;
}

/* ls'in listeleyecegi iki bos dosyayi olusturur. */
static int ornek_dizin_hazirla(void)
{
    if (mkdtemp(dizin) == NULL)
        return -1;
    snprintf(yol1, sizeof yol1, "%s/notlar.txt", dizin);
    snprintf(yol2, sizeof yol2, "%s/odev.c", dizin);
    int a = open(yol1, O_WRONLY | O_CREAT, 0644);
    int b = open(yol2, O_WRONLY | O_CREAT, 0644);
    if (a == -1 || b == -1)
        return -1;
    close(a);
    close(b);
    return 0;
}

int main(void)
{
    char command[256];
    char *parameters[3];
    int status;

    if (ornek_dizin_hazirla() == -1) {
        perror("dizin");
        return 1;
    }

    while (TRUE) {              /* sonsuza dek (girdi bitene kadar) */
        type_prompt();          /* istemi yaz */
        if (!read_command(command, parameters))
            break;
        fflush(stdout);         /* cocuk ile ebeveyn ciktisi karismasin */

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            break;
        }
        if (pid != 0) {         /* ebeveyn */
            waitpid(-1, &status, 0);
            printf("[cocuk %s bitti, cikis durumu %d]\n",
                   parameters[0], WEXITSTATUS(status));
        } else {                /* cocuk surec */
            execve(command, parameters, 0);
            perror("execve");   /* yalnizca execve basarisizsa */
            _exit(127);
        }
    }

    unlink(yol1);
    unlink(yol2);
    rmdir(dizin);
    return 0;
}

/* Beklenen cikti (ls ciktisi bir terminale gitmediginde her ad ayri satirda):
 * $ ls /tmp/os02_kabukXXXXXX     (XXXXXX her calistirmada farkli)
 * notlar.txt
 * odev.c
 * [cocuk ls bitti, cikis durumu 0]
 * $ echo merhaba
 * merhaba
 * [cocuk echo bitti, cikis durumu 0]
 * $ (girdi sonu)
 */
