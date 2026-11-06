/* Semafor bekleme kuyrugu: value ve FIFO surec listesiyle wait (P/down) ve
 * signal (V/up). Cekirdek benzetimi: wait/signal'in atomikligini tek bir
 * "cekirdek" mutex'i saglar (kesmeleri kapatmanin yerine), sleep/wakeup her surecin
 * kendi kosul degiskeniyle yapilir. Slayttaki iz: S = 1; P1 girer, P2 ve P3
 * bloke olur (value = -2), sonra sirayla uyandirilir ve value = 1'e doner. */
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>

/* wait, signal ve sleep libc'de baska anlamlarla da vardir (<sys/wait.h>,
 * <signal.h>, <unistd.h>). Cakisma olmasin diye basliklardan SONRA yeniden
 * adlandirilirlar; kaynakta slayttaki adlar kullanilir. */
#define wait   semafor_wait
#define signal semafor_signal
#define sleep  surec_sleep

struct process {
    int id;
    int uyandirildi;
    pthread_cond_t uyan;
    struct process *next;
};

typedef struct {
    int value;
    struct process *list;
} semaphore;

static pthread_mutex_t cekirdek = PTHREAD_MUTEX_INITIALIZER;
static _Thread_local struct process *bu_surec;

static void ekle(struct process **list, struct process *p)   /* kuyruk sonuna */
{
    p->next = NULL;
    while (*list != NULL)
        list = &(*list)->next;
    *list = p;
}

static struct process *cikar(struct process **list)          /* FIFO: bastan */
{
    struct process *p = *list;
    *list = p->next;
    p->next = NULL;
    return p;
}

static void kuyruk_yaz(const semaphore *S)
{
    printf(" (kuyruk:");
    for (const struct process *p = S->list; p != NULL; p = p->next)
        printf(" P%d", p->id);
    printf(")\n");
}

/* cekirdek kilidi tutulurken cagrilir; uyurken kilit birakilir */
static void sleep(void)
{
    while (!bu_surec->uyandirildi)
        pthread_cond_wait(&bu_surec->uyan, &cekirdek);
    bu_surec->uyandirildi = 0;
}

static void wakeup(struct process *P)
{
    P->uyandirildi = 1;
    pthread_cond_signal(&P->uyan);
}

void wait(semaphore *S)          /* P / down */
{
    pthread_mutex_lock(&cekirdek);
    S->value--;
    printf("P%d wait  : value = %2d", bu_surec->id, S->value);
    if (S->value < 0) {
        ekle(&S->list, bu_surec);
        printf(" -> bloke");
        kuyruk_yaz(S);
        sleep();                 /* bloke ol */
        printf("P%d uyandi -> kritik bolge\n", bu_surec->id);
    } else {
        printf(" -> kritik bolge\n");
    }
    pthread_mutex_unlock(&cekirdek);
}

void signal(semaphore *S)        /* V / up */
{
    struct process *P;
    pthread_mutex_lock(&cekirdek);
    S->value++;
    printf("P%d signal: value = %2d", bu_surec->id, S->value);
    if (S->value <= 0) {
        P = cikar(&S->list);     /* FIFO */
        printf(" -> P%d uyandirildi", P->id);
        kuyruk_yaz(S);
        wakeup(P);
    } else {
        printf(" -> kuyruk bos\n");
    }
    pthread_mutex_unlock(&cekirdek);
}

/* ---- iz senaryosu ---- */
static semaphore S = { 1, NULL };
static atomic_int kb_sahibi = 0;
static atomic_int devam[4];

static void kisa_bekle(void)
{
    struct timespec ts = { 0, 1000000 };   /* 1 ms */
    nanosleep(&ts, NULL);
}

static void *surec(void *arg)
{
    struct process ben;
    ben.id = *(int *)arg;
    ben.uyandirildi = 0;
    ben.next = NULL;
    pthread_cond_init(&ben.uyan, NULL);
    bu_surec = &ben;

    wait(&S);
    atomic_store(&kb_sahibi, ben.id);        /* kritik bolge */
    while (!atomic_load(&devam[ben.id]))
        kisa_bekle();
    signal(&S);

    pthread_cond_destroy(&ben.uyan);
    return NULL;
}

static int deger_oku(void)
{
    pthread_mutex_lock(&cekirdek);
    int v = S.value;
    pthread_mutex_unlock(&cekirdek);
    return v;
}

int main(void)
{
    pthread_t t[4];
    int id[4] = { 0, 1, 2, 3 };

    printf("S.value = %d\n", S.value);
    pthread_create(&t[1], NULL, surec, &id[1]);
    while (atomic_load(&kb_sahibi) != 1)
        kisa_bekle();
    pthread_create(&t[2], NULL, surec, &id[2]);
    while (deger_oku() != -1)
        kisa_bekle();
    pthread_create(&t[3], NULL, surec, &id[3]);
    while (deger_oku() != -2)
        kisa_bekle();
    for (int i = 1; i <= 3; i++) {          /* P1, P2, P3 sirayla cikar */
        while (atomic_load(&kb_sahibi) != i)
            kisa_bekle();
        atomic_store(&devam[i], 1);
    }
    for (int i = 1; i <= 3; i++)
        pthread_join(t[i], NULL);
    printf("S.value = %d\n", S.value);
    return 0;
}

/* Beklenen cikti (her calistirmada ayni):
 * S.value = 1
 * P1 wait  : value =  0 -> kritik bolge
 * P2 wait  : value = -1 -> bloke (kuyruk: P2)
 * P3 wait  : value = -2 -> bloke (kuyruk: P2 P3)
 * P1 signal: value = -1 -> P2 uyandirildi (kuyruk: P3)
 * P2 uyandi -> kritik bolge
 * P2 signal: value =  0 -> P3 uyandirildi (kuyruk:)
 * P3 uyandi -> kritik bolge
 * P3 signal: value =  1 -> kuyruk bos
 * S.value = 1
 */
