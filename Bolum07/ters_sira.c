/* Ters sirada edinim: A once r1 sonra r2, B once r2 sonra r1 ister -> kilitlenme.
 * down() 1 sn zaman asimli sem_timedwait kullanir; boylece kilitlenen is
 * parcaciklari vazgecip biter ve program sonlanir. Ikinci turda B, A bittikten
 * sonra baslar: ayni kod bu sefer kilitlenmez. */
#define _GNU_SOURCE
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define DURUM 96

typedef sem_t semaphore;

/* Sunumda: semaphore r1 = 1, r2 = 1;  (sem_t'ye = 1 yazilamaz; main'de sem_init) */
static semaphore r1, r2;

static int pencere;                       /* 1: down'dan sonra 100 ms uyu */
static char durum_A[DURUM], durum_B[DURUM];
static _Thread_local char *durumum;       /* bu is parcaciginin durum metni */
static _Thread_local int tutulan;         /* bit 1: r1, bit 2: r2 */

static const char *ad(semaphore *s) { return s == &r1 ? "r1" : "r2"; }
static int bit(semaphore *s) { return s == &r1 ? 1 : 2; }

static void down(semaphore *s)
{
    struct timespec son;

    clock_gettime(CLOCK_REALTIME, &son);
    son.tv_sec += 1;
    while (sem_timedwait(s, &son) != 0) {
        if (errno == ETIMEDOUT) {
            snprintf(durumum, DURUM, "bloke (%s bekliyor, %s tutuyor)",
                     ad(s), tutulan == 1 ? "r1" : "r2");
            pthread_exit(NULL);             /* 1 sn icinde alamadi: vazgec */
        }
    }
    tutulan |= bit(s);
    snprintf(durumum, DURUM, "%s tutuyor", ad(s));
    if (pencere)
        usleep(100000);                     /* pencereyi buyut */
}

static void up(semaphore *s)
{
    tutulan &= ~bit(s);
    sem_post(s);
}

static void use_both(void)
{
    snprintf(durumum, DURUM, "iki kaynakla calisti");
}

static void A(void)
{
    down(&r1);
    down(&r2);
    use_both();
    up(&r2); up(&r1);
}

static void B(void)
{
    down(&r2);
    down(&r1);
    use_both();
    up(&r1); up(&r2);
}

static void *calis_A(void *arg)
{
    (void)arg;
    durumum = durum_A;
    A();
    snprintf(durumum, DURUM, "bitti");
    return NULL;
}

static void *calis_B(void *arg)
{
    (void)arg;
    durumum = durum_B;
    B();
    snprintf(durumum, DURUM, "bitti");
    return NULL;
}

static void tur(int es_zamanli)
{
    pthread_t a, b;

    sem_init(&r1, 0, 1);
    sem_init(&r2, 0, 1);
    snprintf(durum_A, DURUM, "hazir");
    snprintf(durum_B, DURUM, "hazir");

    pthread_create(&a, NULL, calis_A, NULL);
    if (!es_zamanli)
        pthread_join(a, NULL);              /* A iki down'u B baslamadan bitirir */
    pthread_create(&b, NULL, calis_B, NULL);
    if (es_zamanli)
        pthread_join(a, NULL);
    pthread_join(b, NULL);

    printf("A: %s\n", durum_A);
    printf("B: %s\n", durum_B);

    sem_destroy(&r1);
    sem_destroy(&r2);
}

int main(void)
{
    printf("1) A ve B ayni anda, down'lar arasinda 100 ms:\n");
    pencere = 1;
    tur(1);
    printf("   -> dongu: A -> r2 -> B -> r1 -> A: kilitlenme\n");

    printf("2) A bitince B baslar:\n");
    pencere = 0;
    tur(0);
    printf("   -> kilitlenme yok\n");
    return 0;
}

/* Beklenen cikti (yaklasik 1,2 sn surer):
 * 1) A ve B ayni anda, down'lar arasinda 100 ms:
 * A: bloke (r2 bekliyor, r1 tutuyor)
 * B: bloke (r1 bekliyor, r2 tutuyor)
 *    -> dongu: A -> r2 -> B -> r1 -> A: kilitlenme
 * 2) A bitince B baslar:
 * A: bitti
 * B: bitti
 *    -> kilitlenme yok
 * Kilitlenme her calistirmada olusmaz; 1. turdaki 100 ms'lik pencere onu
 * neredeyse kesin kilar.
 */
