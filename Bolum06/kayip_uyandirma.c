/* Olumcul yaris: kayip uyandirma (lost wakeup). sleep/wakeup ile yazilmis
 * uretici-tuketici: tuketici count == 0 gorur ama uyumadan once kesilir; uretici
 * wakeup(consumer) gonderir, kimse uyumadigi icin sinyal kaybolur. Sonunda ikisi
 * de uyur. Kesilme anini bir gecikme zorlar; sonsuz uyku yerine sleep() 1 saniyelik
 * zaman asimiyla doner ve program kilitlenmeyi raporlayip biter. */
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>
#include <errno.h>

#define sleep surec_sleep          /* <unistd.h> sleep() ile karismasin */

#define N 100
#define TRUE 1
#define URETIM 1000                /* while (TRUE) yerine en fazla bu kadar oge */

atomic_int count = 0;
static atomic_int tampon[N];
static int ekle_i = 0, al_i = 0;   /* yalniz uretici / yalniz tuketici kullanir */

/* her surecin uyku kaydi */
struct uyku {
    const char *ad;
    pthread_mutex_t m;
    pthread_cond_t c;
    int uyuyor, uyandirildi, kilitlendi;
};
static struct uyku uretici_k = { "uretici", PTHREAD_MUTEX_INITIALIZER,
                                 PTHREAD_COND_INITIALIZER, 0, 0, 0 };
static struct uyku tuketici_k = { "tuketici", PTHREAD_MUTEX_INITIALIZER,
                                  PTHREAD_COND_INITIALIZER, 0, 0, 0 };
static _Thread_local struct uyku *ben;
static atomic_int kayip = 0;
static atomic_int tuketici_okudu = 0;

void *producer(void *arg);
void *consumer(void *arg);

/* sleep(): uyandirilana kadar uyu; 1 s icinde kimse uyandirmazsa kilitlenme say */
static int sleep(void)
{
    struct timespec son;
    clock_gettime(CLOCK_REALTIME, &son);
    son.tv_sec += 1;
    pthread_mutex_lock(&ben->m);
    ben->uyuyor = 1;
    while (!ben->uyandirildi) {
        if (pthread_cond_timedwait(&ben->c, &ben->m, &son) == ETIMEDOUT) {
            ben->kilitlendi = 1;
            break;
        }
    }
    ben->uyuyor = 0;
    ben->uyandirildi = 0;
    int k = ben->kilitlendi;
    pthread_mutex_unlock(&ben->m);
    return k;
}

/* wakeup(kim): kim uyumuyorsa uyandirma bosa gider */
static void wakeup(void *(*kim)(void *))
{
    struct uyku *u = (kim == producer) ? &uretici_k : &tuketici_k;
    pthread_mutex_lock(&u->m);
    if (u->uyuyor) {
        u->uyandirildi = 1;
        pthread_cond_signal(&u->c);
    } else {
        atomic_fetch_add(&kayip, 1);       /* kayip uyandirma */
    }
    pthread_mutex_unlock(&u->m);
}

static int sonraki = 0;
static int produce_item(void) { return ++sonraki; }
static void insert_item(int item) { atomic_store(&tampon[ekle_i], item); ekle_i = (ekle_i + 1) % N; }
static int remove_item(void) { int x = atomic_load(&tampon[al_i]); al_i = (al_i + 1) % N; return x; }
static long tuketilen_toplam = 0;
static void consume_item(int item) { tuketilen_toplam += item; }

/* tuketici count'u okuduktan hemen sonra kesilir (zamanlayici CPU'yu alir) */
static void kesilme(void)
{
    atomic_store(&tuketici_okudu, 1);
    struct timespec ts = { 0, 100000000 };   /* 100 ms */
    nanosleep(&ts, NULL);
}

void *producer(void *arg)
{
    int item;
    (void)arg;
    ben = &uretici_k;
    for (int k = 0; k < URETIM; k++) {      /* while (TRUE) */
        item = produce_item();
        if (atomic_load(&count) == N)
            if (sleep()) return NULL;        /* kilitlendi */
        insert_item(item);
        atomic_store(&count, atomic_load(&count) + 1);   /* count = count + 1 */
        if (atomic_load(&count) == 1) wakeup(consumer);
    }
    return NULL;
}

void *consumer(void *arg)
{
    int item;
    (void)arg;
    ben = &tuketici_k;
    for (int k = 0; k < URETIM; k++) {      /* while (TRUE) */
        if (atomic_load(&count) == 0) {
            kesilme();                       /* <- burada kesilir */
            if (sleep()) return NULL;        /* kilitlendi */
        }
        item = remove_item();
        atomic_store(&count, atomic_load(&count) - 1);   /* count = count - 1 */
        if (atomic_load(&count) == N - 1) wakeup(producer);
        consume_item(item);
    }
    return NULL;
}

int main(void)
{
    pthread_t u, t;
    pthread_create(&t, NULL, consumer, NULL);
    while (!atomic_load(&tuketici_okudu)) {  /* tuketici count == 0 gordu */
        struct timespec ts = { 0, 1000000 };
        nanosleep(&ts, NULL);
    }
    pthread_create(&u, NULL, producer, NULL);
    pthread_join(u, NULL);
    pthread_join(t, NULL);

    printf("count = %d\n", atomic_load(&count));
    printf("kaybolan wakeup sayisi = %d\n", atomic_load(&kayip));
    printf("uretici : %s\n", uretici_k.kilitlendi ? "uyuyor (kimse uyandirmadi)" : "bitti");
    printf("tuketici: %s\n", tuketici_k.kilitlendi ? "uyuyor (kimse uyandirmadi)" : "bitti");
    if (uretici_k.kilitlendi && tuketici_k.kilitlendi)
        printf("ikisi de sonsuza dek uyurdu: kilitlenme\n");
    return 0;
}

/* Beklenen cikti (zamanlamaya bagli ama 100 ms'lik pencere yuzunden pratikte
 * her calistirmada ayni):
 * count = 100
 * kaybolan wakeup sayisi = 1
 * uretici : uyuyor (kimse uyandirmadi)
 * tuketici: uyuyor (kimse uyandirmadi)
 * ikisi de sonsuza dek uyurdu: kilitlenme
 */
