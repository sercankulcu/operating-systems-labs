/* Ayni sira: iki surec iki kaynagi ayni sirada (once resource_1, sonra resource_2)
 * alir. Birakma sirasi farkli olsa bile kilitlenme olmaz (Tanenbaum Sekil 6-2a).
 * down/up, POSIX semaforlariyla (sem_wait/sem_post) gerceklenmistir. */
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

#define TUR 100000

static sem_t resource_1, resource_2;
static long sayac = 0;          /* iki kaynakla korunan ortak veri */

static void down(sem_t *s)
{
    while (sem_wait(s) != 0) {
        /* sinyal kesmesi (EINTR): yeniden dene */
    }
}

static void up(sem_t *s)
{
    sem_post(s);
}

static void use_both_resources(void)
{
    sayac++;
}

static void process_A(void)
{
    down(&resource_1);
    down(&resource_2);
    use_both_resources();
    up(&resource_2);
    up(&resource_1);
}

static void process_B(void)
{
    down(&resource_1);
    down(&resource_2);
    use_both_resources();
    up(&resource_2);
    up(&resource_1);
}

static void *calis_A(void *arg)
{
    (void)arg;
    for (int i = 0; i < TUR; i++)
        process_A();
    return NULL;
}

static void *calis_B(void *arg)
{
    (void)arg;
    for (int i = 0; i < TUR; i++)
        process_B();
    return NULL;
}

int main(void)
{
    pthread_t a, b;

    sem_init(&resource_1, 0, 1);
    sem_init(&resource_2, 0, 1);

    pthread_create(&a, NULL, calis_A, NULL);
    pthread_create(&b, NULL, calis_B, NULL);
    pthread_join(a, NULL);
    pthread_join(b, NULL);

    printf("sayac = %ld (beklenen %d)\n", sayac, 2 * TUR);
    printf("kilitlenme yok: iki surec de bitti\n");

    sem_destroy(&resource_1);
    sem_destroy(&resource_2);
    return 0;
}

/* Beklenen cikti:
 * sayac = 200000 (beklenen 200000)
 * kilitlenme yok: iki surec de bitti
 */
