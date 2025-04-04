#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0
#define N 2 // Number of processes

int turn;
int interested[N] = {FALSE}; // Array to indicate whether each process is interested in the critical section
int favored; // Process favored to enter the critical section

void enter_critical_section(int id) {
    int other = 1 - id; // Index of the other process
    interested[id] = TRUE; // Marking itself as interested
    favored = id; // Marking itself as favored

    // Spin until the other process is not interested or it's favored to enter
    while (interested[other] == TRUE && favored == id)
        ;

    // Critical section
    printf("Process %d entered critical section\n", id);

    // Exit critical section
    interested[id] = FALSE;
}

void *process(void *arg) {
    int id = *((int *)arg);
    for (int i = 0; i < 5; i++) {
        // Non-critical section
        printf("Process %d is in non-critical section\n", id);
        sleep(1);

        // Critical section
        enter_critical_section(id);
        printf("Process %d exited from critical section\n", id);
    }
    return NULL;
}

int main() {
    pthread_t threads[N];
    int thread_ids[N];

    // Creating threads
    for (int i = 0; i < N; i++) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, process, &thread_ids[i]) != 0) {
            perror("pthread_create");
            exit(1);
        }
    }

    // Joining threads
    for (int i = 0; i < N; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            exit(1);
        }
    }

    return 0;
}

