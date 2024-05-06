#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *fifo_thread(void *arg);
void *rr_thread(void *arg);

int main() {
    pthread_t fifo_tid, rr_tid;
    pthread_attr_t fifo_attr, rr_attr;
    struct sched_param fifo_param, rr_param;

    // Initialize attributes
    pthread_attr_init(&fifo_attr);
    pthread_attr_init(&rr_attr);

    // Set scheduling policy to FIFO for fifo_attr
    pthread_attr_setschedpolicy(&fifo_attr, SCHED_FIFO);

    // Set scheduling policy to Round Robin for rr_attr
    pthread_attr_setschedpolicy(&rr_attr, SCHED_RR);

    // Set priority for FIFO thread
    fifo_param.sched_priority = 50; // Set a high priority
    pthread_attr_setschedparam(&fifo_attr, &fifo_param);

    // Set priority for Round Robin thread
    rr_param.sched_priority = 30; // Set a lower priority
    pthread_attr_setschedparam(&rr_attr, &rr_param);

    // Create FIFO thread
    pthread_create(&fifo_tid, &fifo_attr, fifo_thread, NULL);

    // Create Round Robin thread
    pthread_create(&rr_tid, &rr_attr, rr_thread, NULL);

    // Wait for threads to finish
    pthread_join(fifo_tid, NULL);
    pthread_join(rr_tid, NULL);

    return 0;
}

void *fifo_thread(void *arg) {
    printf("FIFO Thread running...\n");
    sleep(3);
    printf("FIFO Thread finished.\n");
    pthread_exit(NULL);
}

void *rr_thread(void *arg) {
    printf("Round Robin Thread running...\n");
    sleep(3);
    printf("Round Robin Thread finished.\n");
    pthread_exit(NULL);
}

