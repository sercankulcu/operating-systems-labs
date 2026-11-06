/* Tek kosul degiskeni yetmez: tek yuvali tampon, iki tuketici (T1, T2), bir
 * uretici (U). Tek kosul c ile tuketicinin signal'i yanlislikla diger tuketiciyi
 * uyandirabilir; sonunda uc is parcacigi da c'de uyur. Iki kosulla (dolu, bos)
 * her signal dogru grubu uyandirir. Sonucu deterministik yapmak icin kosul
 * degiskeni FIFO kuyruklu olarak elle yazildi (pthread_cond hangi bekleyeni
 * uyandiracagini garanti etmez). wait 1 s icinde uyanmazsa kilitlenme sayilir. */
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>

/* wait/signal <sys/wait.h> ve <signal.h> ile karismasin */
#define wait   kosul_wait
#define signal kosul_signal

struct bekleyen {
    sem_t s;
    struct bekleyen *next;
};
typedef struct {
    struct bekleyen *bas;        /* FIFO bekleme kuyrugu */
} kosul_t;

static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static int kilitlendi = 0;

/* m tutulurken cagrilir: kuyruga gir, m'yi birak, uyu, m'yi yeniden al */
static void wait(kosul_t *c, pthread_mutex_t *mx)
{
    struct bekleyen b;
    struct bekleyen **p = &c->bas;
    sem_init(&b.s, 0, 0);
    b.next = NULL;
    while (*p != NULL)
        p = &(*p)->next;
    *p = &b;
    pthread_mutex_unlock(mx);

    struct timespec son;
    clock_gettime(CLOCK_REALTIME, &son);
    son.tv_sec += 1;
    int uyandirildi = 1;
    while (sem_timedwait(&b.s, &son) != 0) {
        if (errno == ETIMEDOUT) {
            uyandirildi = 0;
            break;
        }
    }
    if (uyandirildi) {
        /* Mesa: uyanan yalniz hazir kuyruguna girer; o calisana dek signal
         * veren devam eder. Bu sirayi her calistirmada ayni yapmak icin
         * uyanan 10 ms gec kalir. */
        struct timespec gecik = { 0, 10000000 };
        nanosleep(&gecik, NULL);
    }
    pthread_mutex_lock(mx);
    if (!uyandirildi) {
        int kuyrukta = 0;
        for (struct bekleyen **q = &c->bas; *q != NULL; q = &(*q)->next) {
            if (*q == &b) {
                *q = b.next;
                kuyrukta = 1;
                break;
            }
        }
        if (kuyrukta)
            kilitlendi = 1;          /* kimse uyandirmadi */
        else
            sem_wait(&b.s);          /* tam zaman asiminda signal geldi */
    }
    sem_destroy(&b.s);
}

/* m tutulurken cagrilir: kuyruktaki ilk bekleyeni uyandir */
static void signal(kosul_t *c)
{
    struct bekleyen *b = c->bas;
    if (b != NULL) {
        c->bas = b->next;
        sem_post(&b->s);
    }
}

static int kuyruk_boyu(kosul_t *c)
{
    int n = 0;
    for (struct bekleyen *b = c->bas; b != NULL; b = b->next)
        n++;
    return n;
}

/* ---- tek yuvali tampon ---- */
static kosul_t c, dolu, bos;
static int iki_kosul;
static int count = 0, yuva = 0, sonraki = 0, tuketilen = 0;

static void koy(int x) { yuva = x; count = 1; }
static int al(void) { count = 0; return yuva; }

static void *tuketici(void *arg)
{
    (void)arg;
    for (int k = 0; k < 2; k++) {
        int x;
        pthread_mutex_lock(&m);
        if (!iki_kosul) {
            /* YANLIS: tek kosul c */
            while (count == 0 && !kilitlendi) wait(&c, &m);
            if (kilitlendi) { pthread_mutex_unlock(&m); return NULL; }
            x = al();  signal(&c);
        } else {
            /* DOGRU: iki kosul */
            while (count == 0 && !kilitlendi) wait(&dolu, &m);
            if (kilitlendi) { pthread_mutex_unlock(&m); return NULL; }
            x = al();  signal(&bos);
        }
        (void)x;
        tuketilen++;
        pthread_mutex_unlock(&m);
    }
    return NULL;
}

static void *uretici(void *arg)
{
    (void)arg;
    for (int k = 0; k < 4; k++) {
        int x = ++sonraki;
        pthread_mutex_lock(&m);
        if (!iki_kosul) {
            while (count == 1 && !kilitlendi) wait(&c, &m);
            if (kilitlendi) { pthread_mutex_unlock(&m); return NULL; }
            koy(x);    signal(&c);
        } else {
            while (count == 1 && !kilitlendi) wait(&bos, &m);
            if (kilitlendi) { pthread_mutex_unlock(&m); return NULL; }
            koy(x);    signal(&dolu);
        }
        pthread_mutex_unlock(&m);
    }
    return NULL;
}

static void kuyruk_bekle(kosul_t *k, int n)
{
    for (;;) {
        pthread_mutex_lock(&m);
        int boy = kuyruk_boyu(k);
        pthread_mutex_unlock(&m);
        if (boy == n)
            return;
        struct timespec ts = { 0, 1000000 };
        nanosleep(&ts, NULL);
    }
}

static void senaryo(int iki)
{
    pthread_t t1, t2, u;
    kosul_t *tk = iki ? &dolu : &c;     /* tuketicilerin bekledigi kosul */
    iki_kosul = iki;
    kilitlendi = 0;
    count = 0; yuva = 0; sonraki = 0; tuketilen = 0;
    pthread_create(&t1, NULL, tuketici, NULL);
    kuyruk_bekle(tk, 1);                /* T1 bekliyor */
    pthread_create(&t2, NULL, tuketici, NULL);
    kuyruk_bekle(tk, 2);                /* T1, T2 bekliyor */
    pthread_create(&u, NULL, uretici, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(u, NULL);
    printf("%s: uretilen = %d, tuketilen = %d -> %s\n",
           iki ? "iki kosul (dolu, bos)" : "tek kosul c          ",
           sonraki, tuketilen,
           kilitlendi ? "KILITLENME: hepsi c'de uyuyor" : "tamam");
}

int main(void)
{
    senaryo(0);
    senaryo(1);
    return 0;
}

/* Beklenen cikti (her calistirmada ayni; ilk senaryo zaman asimi yuzunden ~1 s):
 * tek kosul c          : uretilen = 2, tuketilen = 1 -> KILITLENME: hepsi c'de uyuyor
 * iki kosul (dolu, bos): uretilen = 4, tuketilen = 4 -> tamam
 *
 * Tek kosuldaki sira: c = [T1 T2]; U koyar, signal -> T1. U tekrar gelir, yuva
 * dolu: c = [T2 U]. T1 alir, signal -> T2 (yanlis grup!). T2 bos gorur, bekler;
 * T1 de bekler: c = [U T1 T2] ya da [U T2 T1]. Artik kimse signal veremez.
 */
