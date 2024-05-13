#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUFFER_SIZE 5
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 2
#define NUM_ITEMS 10

int buffer[BUFFER_SIZE];
int in = 0, out = 0;
int count = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
    int item;
    for (int i = 0; i < NUM_ITEMS; i++) {
        item = rand() % 100 + (i * 1000); // Generate a random item
        pthread_mutex_lock(&mutex);
        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&full, &mutex); // Wait for the buffer to be not full
        }
        buffer[in] = item;
        printf("Produced item %d\n", item);
        in = (in + 1) % BUFFER_SIZE;
        count++;
        pthread_cond_signal(&empty); // Signal that the buffer is not empty
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

void *consumer(void *arg) {
    int item;
    for (int i = 0; i < NUM_ITEMS; i++) {
        pthread_mutex_lock(&mutex);
        while (count == 0) {
            pthread_cond_wait(&empty, &mutex); // Wait for the buffer to be not empty
        }
        item = buffer[out];
        printf("Consumed item %d\n", item);
        out = (out + 1) % BUFFER_SIZE;
        count--;
        pthread_cond_signal(&full); // Signal that the buffer is not full
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];

    // Create producer threads
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        if (pthread_create(&producers[i], NULL, producer, NULL) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    // Create consumer threads
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        if (pthread_create(&consumers[i], NULL, consumer, NULL) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for producer threads to finish
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        if (pthread_join(producers[i], NULL) != 0) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for consumer threads to finish
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        if (pthread_join(consumers[i], NULL) != 0) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);

    return 0;
}

