/* Is parcacigi cizelgeleme kapsami: PCS (surec ici yarisma) ve SCS (sistem
 * capinda yarisma). 1:1 modelli Linux yalnizca PTHREAD_SCOPE_SYSTEM'i
 * destekler; PTHREAD_SCOPE_PROCESS istegi ENOTSUP ile reddedilir. */
#include <stdio.h>
#include <string.h>
#include <pthread.h>

static void *runner(void *param)
{
    (void)param;
    printf("runner calisti\n");
    return NULL;
}

int main(void)
{
    pthread_t tid;
    pthread_attr_t attr;
    int scope;
    pthread_attr_init(&attr);
    pthread_attr_getscope(&attr, &scope);
    if (scope == PTHREAD_SCOPE_PROCESS)
        printf("PCS\n");
    else if (scope == PTHREAD_SCOPE_SYSTEM)
        printf("SCS\n");

    int r = pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);
    printf("PROCESS kapsami: %s\n", r == 0 ? "kabul edildi" : strerror(r));

    /* Linux ve macOS yalnizca SYSTEM */
    pthread_attr_setscope(&attr,
                          PTHREAD_SCOPE_SYSTEM);
    pthread_create(&tid, &attr, runner, NULL);
    pthread_join(tid, NULL);
    pthread_attr_destroy(&attr);
    return 0;
}

/* Beklenen cikti (Linux/glibc):
 * SCS
 * PROCESS kapsami: Operation not supported
 * runner calisti
 */
