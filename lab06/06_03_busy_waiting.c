#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>

#define NUM_THREADS 2

atomic_int turn = ATOMIC_VAR_INIT(0);
atomic_int flag[NUM_THREADS];

void *thread_function(void *arg);

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS] = {0, 1};

    // Initialize flags
    for (int i = 0; i < NUM_THREADS; i++) {
        flag[i] = 0;
    }

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_function, (void *)&thread_ids[i]);
    }

    // Wait for threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

void *thread_function(void *arg) {
    int thread_id = *((int *)arg);

    for (int i = 0; i < 5; i++) { // Perform 5 iterations

        flag[thread_id] = 1;
        while (flag[1 - thread_id] == 1) {
            if (turn != thread_id) {
                flag[thread_id] = 0;
                while (turn != thread_id) {} // Busy wait
                flag[thread_id] = 1;
            }
        }

        // Critical section
        printf("Thread %d is in its critical section.\n", thread_id);
        // Simulate some work
        sleep(1);

        turn = 1 - thread_id; // Change turn

        // Exit critical section
        flag[thread_id] = 0;
    }

    pthread_exit(NULL);
}

