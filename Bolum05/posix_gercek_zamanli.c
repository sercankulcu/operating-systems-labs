/* POSIX gercek zamanli cizelgeleme: is parcacigi SCHED_FIFO politikasi ve
 * 50 onceligiyle olusturulmak istenir. Bu ayricalik ister (root ya da
 * CAP_SYS_NICE); yetki yoksa pthread_create EPERM dondurur ve program
 * varsayilan politikayla devam eder. Kabukta karsiligi: chrt -f 50 ./prog */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>

static void *runner(void *param)
{
    (void)param;
    int policy;
    struct sched_param sp;
    pthread_getschedparam(pthread_self(), &policy, &sp);
    printf("runner: politika=%s oncelik=%d\n",
           policy == SCHED_FIFO ? "SCHED_FIFO" :
           policy == SCHED_RR ? "SCHED_RR" : "SCHED_OTHER",
           sp.sched_priority);
    return NULL;
}

int main(void)
{
    pthread_t tid;
    pthread_attr_t attr;
    struct sched_param sp;
    int policy;
    pthread_attr_init(&attr);
    pthread_attr_getschedpolicy(&attr, &policy);
    printf("varsayilan politika: %s\n",
           policy == SCHED_OTHER ? "SCHED_OTHER" : "baska");
    /* kalitim kapatilmazsa politika yok sayilir */
    pthread_attr_setinheritsched(&attr,
                          PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    sp.sched_priority = 50;          /* 1..99 */
    pthread_attr_setschedparam(&attr, &sp);
    int r = pthread_create(&tid, &attr, runner, NULL);
    if (r != 0) {               /* EPERM beklenir */
        printf("SCHED_FIFO %s (%s); varsayilan politikayla\n",
               r == EPERM ? "icin yetki yok" : "reddedildi", strerror(r));
        r = pthread_create(&tid, NULL, runner, NULL);
    }
    if (r != 0) {
        printf("pthread_create: %s\n", strerror(r));
        return 1;
    }
    pthread_join(tid, NULL);
    pthread_attr_destroy(&attr);
    return 0;
}

/* Beklenen cikti (siradan kullanici olarak):
 * varsayilan politika: SCHED_OTHER
 * SCHED_FIFO icin yetki yok (Operation not permitted); varsayilan politikayla
 * runner: politika=SCHED_OTHER oncelik=0
 * root ya da CAP_SYS_NICE ile ikinci ve ucuncu satirin yerine:
 * runner: politika=SCHED_FIFO oncelik=50
 */
