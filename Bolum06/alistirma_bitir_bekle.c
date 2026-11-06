/* Alistirmalar: ebeveyn bekle(), cocuk is parcacigi isi bitince bitir() cagirir.
 * (a) durum degiskeni yok: cocuk once biterse sinyal kaybolur.
 * (b) kilit yok, if var: sinama ile uyuma arasinda sinyal kaybolur.
 * Dogru surum: bitti degiskeni + kilit + while. Son olarak tek mutex ve tek kosul
 * degiskeniyle sem_bekle / sem_ver. Sonsuz uyku yerine wait 500 ms'de doner. */
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>
#include <errno.h>

static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t c = PTHREAD_COND_INITIALIZER;
static int zaman_asimi = 0;          /* bir wait kimse uyandirmadan dondu mu */

static void lock(pthread_mutex_t *x)   { pthread_mutex_lock(x); }
static void unlock(pthread_mutex_t *x) { pthread_mutex_unlock(x); }
static void kosul_signal(pthread_cond_t *k) { pthread_cond_signal(k); }

static void son_an(struct timespec *son)
{
    clock_gettime(CLOCK_REALTIME, son);
    son->tv_nsec += 500000000L;      /* 500 ms */
    if (son->tv_nsec >= 1000000000L) {
        son->tv_sec += 1;
        son->tv_nsec -= 1000000000L;
    }
}

/* wait(&c, &m): kilitle birlikte (500 ms sinirli) */
static void kosul_wait2(pthread_cond_t *k, pthread_mutex_t *x)
{
    struct timespec son;
    son_an(&son);
    if (pthread_cond_timedwait(k, x, &son) == ETIMEDOUT)
        zaman_asimi = 1;
}

/* wait(&c): (b)'deki gibi paylasilan kilit olmadan; pthread bir mutex istedigi
 * icin yalniz bu cagriya ozel bir mutex kullanilir (bu, kilit yok demektir) */
static void kosul_wait1(pthread_cond_t *k)
{
    pthread_mutex_t ozel;
    pthread_mutex_init(&ozel, NULL);
    pthread_mutex_lock(&ozel);
    kosul_wait2(k, &ozel);
    pthread_mutex_unlock(&ozel);
    pthread_mutex_destroy(&ozel);
}

/* Slayttaki wait(&c, &m) ve wait(&c) yazimlari: arguman sayisina gore secilir.
 * Adlar <sys/wait.h> ve <signal.h> ile karismasin diye makrodur. */
#define WAIT_SEC(_1, _2, AD, ...) AD
#define wait(...) WAIT_SEC(__VA_ARGS__, kosul_wait2, kosul_wait1, 0)(__VA_ARGS__)
#define signal kosul_signal

/* ---- (a) durum degiskeni yok ---- */
void bitir_a(void)
{
    lock(&m);
    signal(&c);
    unlock(&m);
}

void bekle_a(void)
{
    lock(&m);
    wait(&c, &m);
    unlock(&m);
}

/* ---- (b) kilit yok, if var ---- */
static volatile int bitti = 0;
static atomic_int sinadi = 0, cocuk_bitirdi = 0;

void bitir_b(void)
{
    bitti = 1;
    signal(&c);
}

void bekle_b(void)
{
    if (bitti == 0) {
        /* <- ebeveyn burada kesilir: cocuk bitti = 1 yapip bosa sinyal verir */
        atomic_store(&sinadi, 1);
        while (!atomic_load(&cocuk_bitirdi)) {
            struct timespec ts = { 0, 1000000 };
            nanosleep(&ts, NULL);
        }
        wait(&c);
    }
}

/* ---- dogru surum ---- */
static int bitti_d = 0;

void bitir(void)
{
    lock(&m);
    bitti_d = 1;               /* olay saklanir */
    signal(&c);
    unlock(&m);
}

void bekle(void)
{
    lock(&m);
    while (bitti_d == 0)       /* sinama ve uyuma ayni kilit altinda */
        wait(&c, &m);
    unlock(&m);
}

/* ---- sem_bekle / sem_ver: tek mutex + tek kosul ---- */
static int deger = 0;

void sem_bekle(void)
{
    lock(&m);
    while (deger <= 0)
        wait(&c, &m);
    deger--;
    unlock(&m);
}

void sem_ver(void)
{
    lock(&m);
    deger++;
    signal(&c);
    unlock(&m);
}

/* ---- cocuk is parcaciklari ---- */
static void *cocuk_a(void *arg) { (void)arg; bitir_a(); return NULL; }
static void *cocuk_b(void *arg)
{
    (void)arg;
    while (!atomic_load(&sinadi)) {
        struct timespec ts = { 0, 1000000 };
        nanosleep(&ts, NULL);
    }
    bitir_b();
    atomic_store(&cocuk_bitirdi, 1);
    return NULL;
}
static void *cocuk_d(void *arg) { (void)arg; bitir(); return NULL; }
static atomic_int alinan = 0;
static void *sem_isci(void *arg) { (void)arg; sem_bekle(); atomic_fetch_add(&alinan, 1); return NULL; }

int main(void)
{
    pthread_t t;

    /* (a) cocuk, ebeveyn bekle()'ye girmeden biter */
    zaman_asimi = 0;
    pthread_create(&t, NULL, cocuk_a, NULL);
    pthread_join(t, NULL);
    bekle_a();
    printf("(a) %s\n", zaman_asimi ? "sinyal kayboldu: ebeveyn hic uyanmazdi"
                                   : "ebeveyn uyandi");

    /* (b) ebeveyn sinar, kesilir, sonra uyur */
    zaman_asimi = 0;
    pthread_create(&t, NULL, cocuk_b, NULL);
    bekle_b();
    pthread_join(t, NULL);
    printf("(b) %s\n", zaman_asimi ? "sinyal kayboldu: ebeveyn hic uyanmazdi"
                                   : "ebeveyn uyandi");

    /* dogru surum: cocuk once bitse de olay bitti_d'de saklanir */
    zaman_asimi = 0;
    pthread_create(&t, NULL, cocuk_d, NULL);
    pthread_join(t, NULL);
    bekle();
    printf("dogru: %s\n", zaman_asimi ? "zaman asimi" : "ebeveyn hemen devam etti");

    /* sem_bekle / sem_ver */
    pthread_t s[3];
    for (int i = 0; i < 3; i++)
        pthread_create(&s[i], NULL, sem_isci, NULL);
    for (int i = 0; i < 3; i++)
        sem_ver();
    for (int i = 0; i < 3; i++)
        pthread_join(s[i], NULL);
    printf("sem_bekle/sem_ver: %d is parcacigi gecti, deger = %d\n",
           atomic_load(&alinan), deger);
    return 0;
}

/* Beklenen cikti (her calistirmada ayni; (a) ve (b) 500'er ms surer):
 * (a) sinyal kayboldu: ebeveyn hic uyanmazdi
 * (b) sinyal kayboldu: ebeveyn hic uyanmazdi
 * dogru: ebeveyn hemen devam etti
 * sem_bekle/sem_ver: 3 is parcacigi gecti, deger = 0
 */
