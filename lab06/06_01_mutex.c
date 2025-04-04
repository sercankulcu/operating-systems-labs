#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int shared_variable = 0;

void *thread_function();

int main() {
    pthread_t tid1, tid2;

    // Create two threads
    pthread_create(&tid1, NULL, thread_function, NULL);
    pthread_create(&tid2, NULL, thread_function, NULL);

    // Wait for threads to finish
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    // Destroy mutex
    pthread_mutex_destroy(&mutex);

    return 0;
}

void *thread_function() {
    
    for(int i = 0; i < 10; i++) {
      // Disable interrupts (mutex lock)
      pthread_mutex_lock(&mutex);

      // Critical section
      printf("Thread ID %ld is in the critical section.\n", pthread_self());
      shared_variable++;
      printf("Shared variable incremented by Thread ID %ld: %d\n", pthread_self(), shared_variable);

      // Simulate some work
      // sleep(1);

      // Re-enable interrupts (mutex unlock)
      pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

