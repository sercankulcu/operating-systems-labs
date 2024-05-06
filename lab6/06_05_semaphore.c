#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_ITEMS 10

int buffer[BUFFER_SIZE];
int in = 0, out = 0;

sem_t empty;   // Semaphore to count empty slots in the buffer
sem_t full;    // Semaphore to count filled slots in the buffer
pthread_mutex_t mutex; // Mutex for buffer access

void *producer(void *arg);
void *consumer(void *arg);

int main() {
    pthread_t producer_tid, consumer_tid;

    // Initialize semaphores and mutex
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    // Create producer and consumer threads
    pthread_create(&producer_tid, NULL, producer, NULL);
    pthread_create(&consumer_tid, NULL, consumer, NULL);

    // Wait for threads to finish
    pthread_join(producer_tid, NULL);
    pthread_join(consumer_tid, NULL);

    // Destroy semaphores and mutex
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    return 0;
}

void *producer(void *arg) {
    for (int i = 0; i < NUM_ITEMS; i++) {
        sem_wait(&empty); // Wait for an empty slot in the buffer
        pthread_mutex_lock(&mutex);

        // Produce item and add it to buffer
        buffer[in] = i;
        printf("Produced item: %d\n", buffer[in]);
        in = (in + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&full); // Signal that buffer has a filled slot
        sleep(1); // Simulate production time
    }
    pthread_exit(NULL);
}

void *consumer(void *arg) {
    for (int i = 0; i < NUM_ITEMS; i++) {
        sem_wait(&full); // Wait for a filled slot in the buffer
        pthread_mutex_lock(&mutex);

        // Consume item from buffer
        printf("Consumed item: %d\n", buffer[out]);
        out = (out + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&empty); // Signal that buffer has an empty slot
        sleep(2); // Simulate consumption time
    }
    pthread_exit(NULL);
}

