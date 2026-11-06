/* Yemek yiyen filozoflar, Tanenbaum cozumu: her filozofun durumu (THINKING,
 * HUNGRY, EATING) ve kendi semaforu s[i] vardir; filozof ancak iki komsusu da
 * yemiyorken yer. Once slayttaki iz (1 ve 3 yer, 2 bekler, 1 ve 3 birakinca 2 yer)
 * oynatilir, sonra bes filozof 1000'er kez yer ve kural denetlenir. */
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <time.h>

#define N        5
#define LEFT     ((i + N - 1) % N)   /* (i+4) % 5 */
#define RIGHT    ((i + 1) % N)       /* (i+1) % 5 */
#define THINKING 0
#define HUNGRY   1
#define EATING   2
#define YEMEK    1000

typedef sem_t semaphore;
static void down(semaphore *x) { sem_wait(x); }
static void up(semaphore *x)   { sem_post(x); }

int state[N];
semaphore mutex;           /* = 1 */
semaphore s[N];            /* hepsi 0 */

void test(int i)
{
    if (state[i] == HUNGRY && state[LEFT] != EATING
                           && state[RIGHT] != EATING) {
        state[i] = EATING;
        up(&s[i]);
    }
}

void take_forks(int i)
{
    down(&mutex);
    state[i] = HUNGRY;
    test(i);
    up(&mutex);
    down(&s[i]);    /* catal yoksa bloke */
}

void put_forks(int i)
{
    down(&mutex);
    state[i] = THINKING;
    test(LEFT);     /* (i+4) % 5 */
    test(RIGHT);    /* (i+1) % 5 */
    up(&mutex);
}

static void durum_yaz(const char *olay)
{
    char d[N + 1];
    down(&mutex);
    for (int k = 0; k < N; k++)
        d[k] = state[k] == THINKING ? 'T' : state[k] == HUNGRY ? 'H' : 'E';
    up(&mutex);
    d[N] = '\0';
    printf("state = %s%s%s\n", d, olay[0] != '\0' ? "   " : "", olay);
}

static void *iki_numara(void *arg)       /* izde 2. filozof */
{
    (void)arg;
    take_forks(2);                       /* 1 yedigi icin bloke olur */
    return NULL;
}

/* ---- serbest kosu ---- */
static int yedi[N];
static atomic_int ihlal = 0;

static void ye(int i)
{
    down(&mutex);
    if (state[LEFT] == EATING || state[RIGHT] == EATING || state[i] != EATING)
        atomic_fetch_add(&ihlal, 1);
    up(&mutex);
    yedi[i]++;
}

static void *filozof(void *arg)
{
    int i = *(int *)arg;
    for (int k = 0; k < YEMEK; k++) {   /* while (true) yerine YEMEK tur */
        /* dusun */
        take_forks(i);
        ye(i);
        put_forks(i);
    }
    return NULL;
}

int main(void)
{
    sem_init(&mutex, 0, 1);
    for (int i = 0; i < N; i++)
        sem_init(&s[i], 0, 0);

    /* 1) Slayttaki iz */
    durum_yaz("");
    take_forks(1);
    durum_yaz("1 catallari aldi, yiyor");
    pthread_t t2;
    pthread_create(&t2, NULL, iki_numara, NULL);
    for (;;) {                                   /* 2 HUNGRY olana dek */
        down(&mutex);
        int h = state[2] == HUNGRY;
        up(&mutex);
        if (h)
            break;
        struct timespec ts = { 0, 1000000 };
        nanosleep(&ts, NULL);
    }
    durum_yaz("2 ac: komsusu 1 yiyor, s[2]'de bloke");
    take_forks(3);
    durum_yaz("3 yiyor (komsulari 2 ve 4 yemiyor)");
    put_forks(1);
    durum_yaz("1 birakti: test(2) basarisiz, 3 yiyor");
    put_forks(3);
    pthread_join(t2, NULL);                      /* test(2) basarili: 2 uyandi */
    durum_yaz("3 birakti: test(2) basarili, 2 yiyor");
    put_forks(2);

    /* 2) Serbest kosu */
    pthread_t t[N];
    int id[N];
    for (int i = 0; i < N; i++) {
        id[i] = i;
        pthread_create(&t[i], NULL, filozof, &id[i]);
    }
    for (int i = 0; i < N; i++)
        pthread_join(t[i], NULL);
    printf("yemek sayilari:");
    for (int i = 0; i < N; i++)
        printf(" %d", yedi[i]);
    printf("\nkomsu ayni anda yedi: %d kez\n", atomic_load(&ihlal));

    sem_destroy(&mutex);
    for (int i = 0; i < N; i++)
        sem_destroy(&s[i]);
    return 0;
}

/* Beklenen cikti (her calistirmada ayni):
 * state = TTTTT
 * state = TETTT   1 catallari aldi, yiyor
 * state = TEHTT   2 ac: komsusu 1 yiyor, s[2]'de bloke
 * state = TEHET   3 yiyor (komsulari 2 ve 4 yemiyor)
 * state = TTHET   1 birakti: test(2) basarisiz, 3 yiyor
 * state = TTETT   3 birakti: test(2) basarili, 2 yiyor
 * yemek sayilari: 1000 1000 1000 1000 1000
 * komsu ayni anda yedi: 0 kez
 */
