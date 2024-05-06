#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 2

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int turn = 0;

void *thread_function(void *arg);

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS] = {0, 1};

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_function, (void *)&thread_ids[i]);
    }

    // Wait for threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destroy mutex
    pthread_mutex_destroy(&mutex);

    return 0;
}

void *thread_function(void *arg) {
    int thread_id = *((int *)arg);

    for (int i = 0; i < 5; i++) { // Perform 5 iterations

        while (1) {
            pthread_mutex_lock(&mutex);
            if (turn == thread_id) {
                printf("Thread %d is in its critical section.\n", thread_id);
                turn = (turn + 1) % NUM_THREADS; // Change turn
                pthread_mutex_unlock(&mutex);
                break;
            }
            pthread_mutex_unlock(&mutex);
        }

        // Critical section
        printf("Thread %d executing...\n", thread_id);
        // Simulate some work
        sleep(1);
    }

    pthread_exit(NULL);
}

