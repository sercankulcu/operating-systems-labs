/* POSIX semaforlari. Adsiz (sem_init): ayni surecin is parcaciklari arasinda,
 * bos/dolu ile N yuvali tampon. Adlandirilmis (sem_open): ilgisiz surecler
 * arasinda; burada fork edilen cocuk semaforu adiyla yeniden acar ve iki surec
 * paylasilan bellekteki sayaci 10000'er kez artirir. */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define N 4
#define ADET 20
#define TUR 10000

/* adsiz: ayni surecin is parcaciklari */
sem_t bos, dolu;
static int tampon[N], giris = 0, cikis = 0;
static long toplam = 0;

static void *uretici(void *arg)
{
    (void)arg;
    for (int x = 1; x <= ADET; x++) {
        sem_wait(&bos);   tampon[giris] = x; giris = (giris + 1) % N;   sem_post(&dolu);
    }
    return NULL;
}

static void *tuketici(void *arg)
{
    (void)arg;
    for (int k = 0; k < ADET; k++) {
        sem_wait(&dolu);  toplam += tampon[cikis]; cikis = (cikis + 1) % N;  sem_post(&bos);
    }
    return NULL;
}

static void artir(sem_t *s, volatile long *sayac)
{
    for (int k = 0; k < TUR; k++) {
        sem_wait(s);  /* KB */
        *sayac = *sayac + 1;
        sem_post(s);
    }
}

int main(void)
{
    /* 1) adsiz semaforlar */
    pthread_t u, t;
    sem_init(&bos,  0, N);   /* 0: surec ici */
    sem_init(&dolu, 0, 0);
    pthread_create(&u, NULL, uretici, NULL);
    pthread_create(&t, NULL, tuketici, NULL);
    pthread_join(u, NULL);
    pthread_join(t, NULL);
    sem_destroy(&bos);
    sem_destroy(&dolu);
    printf("adsiz: tuketilen toplam = %ld (1 + ... + %d)\n", toplam, ADET);

    /* 2) adlandirilmis semafor: ilgisiz surecler arasi */
    sem_unlink("/os06");                 /* onceki calistirmadan kalmissa */
    sem_t *s = sem_open("/os06", O_CREAT, 0644, 1);
    if (s == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }
    void *bellek = mmap(NULL, sizeof(long), PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (bellek == MAP_FAILED) {
        perror("mmap");
        sem_close(s);
        sem_unlink("/os06");
        return 1;
    }
    volatile long *sayac = bellek;     /* iki surecin paylastigi sayac */
    *sayac = 0;

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        sem_close(s);
        sem_unlink("/os06");
        munmap(bellek, sizeof(long));
        return 1;
    }
    if (pid == 0) {                      /* cocuk: semaforu adiyla acar */
        sem_t *cs = sem_open("/os06", 0);
        if (cs == SEM_FAILED)
            _exit(1);
        artir(cs, sayac);
        sem_close(cs);
        _exit(0);
    }
    artir(s, sayac);
    int durum = 0;
    waitpid(pid, &durum, 0);

    printf("adlandirilmis: sayac = %ld (2 surec x %d)\n", *sayac, TUR);
    printf("cocuk cikis kodu = %d\n", WIFEXITED(durum) ? WEXITSTATUS(durum) : -1);
    sem_close(s);
    sem_unlink("/os06");
    munmap(bellek, sizeof(long));
    return 0;
}

/* Beklenen cikti (her calistirmada ayni):
 * adsiz: tuketilen toplam = 210 (1 + ... + 20)
 * adlandirilmis: sayac = 20000 (2 surec x 10000)
 * cocuk cikis kodu = 0
 */
