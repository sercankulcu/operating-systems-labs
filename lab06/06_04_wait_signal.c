#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_ITEMS 10

int buffer[BUFFER_SIZE];
int in = 0, out = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_full = PTHREAD_COND_INITIALIZER;

void *producer();
void *consumer();

int main() {
    pthread_t producer_tid, consumer_tid;

    // Create producer and consumer threads
    pthread_create(&producer_tid, NULL, producer, NULL);
    pthread_create(&consumer_tid, NULL, consumer, NULL);

    // Wait for threads to finish
    pthread_join(producer_tid, NULL);
    pthread_join(consumer_tid, NULL);

    // Destroy mutex and condition variables
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_empty);
    pthread_cond_destroy(&cond_full);

    return 0;
}

void *producer() {
    for (int i = 0; i < NUM_ITEMS; i++) {
        pthread_mutex_lock(&mutex);

        while ((in + 1) % BUFFER_SIZE == out) { // Buffer is full
            pthread_cond_wait(&cond_full, &mutex); // Wait for consumer to consume
        }

        // Produce item and add it to buffer
        buffer[in] = i;
        printf("Produced item: %d\n", buffer[in]);
        in = (in + 1) % BUFFER_SIZE;

        pthread_cond_signal(&cond_empty); // Signal to consumer that buffer is not empty
        pthread_mutex_unlock(&mutex);
        sleep(1); // Simulate production time
    }
    pthread_exit(NULL);
}

void *consumer() {
    for (int i = 0; i < NUM_ITEMS; i++) {
        pthread_mutex_lock(&mutex);

        while (in == out) { // Buffer is empty
            pthread_cond_wait(&cond_empty, &mutex); // Wait for producer to produce
        }

        // Consume item from buffer
        printf("Consumed item: %d\n", buffer[out]);
        out = (out + 1) % BUFFER_SIZE;

        pthread_cond_signal(&cond_full); // Signal to producer that buffer is not full
        pthread_mutex_unlock(&mutex);
        sleep(2); // Simulate consumption time
    }
    pthread_exit(NULL);
}

