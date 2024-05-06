#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM_READERS 3
#define NUM_WRITERS 2

int data = 0; // Shared data
int readers_count = 0; // Number of readers currently reading
sem_t mutex, write_mutex; // Semaphores for mutual exclusion

void *reader(void *arg);
void *writer(void *arg);

int main() {
    pthread_t reader_threads[NUM_READERS], writer_threads[NUM_WRITERS];

    // Initialize semaphores
    sem_init(&mutex, 0, 1);
    sem_init(&write_mutex, 0, 1);

    // Create reader threads
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_create(&reader_threads[i], NULL, reader, NULL);
    }

    // Create writer threads
    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_create(&writer_threads[i], NULL, writer, NULL);
    }

    // Join reader threads
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(reader_threads[i], NULL);
    }

    // Join writer threads
    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writer_threads[i], NULL);
    }

    // Destroy semaphores
    sem_destroy(&mutex);
    sem_destroy(&write_mutex);

    return 0;
}

void *reader(void *arg) {
    while (1) {
        // Reader entry section
        sem_wait(&mutex);
        readers_count++;
        if (readers_count == 1) { // First reader
            sem_wait(&write_mutex); // Prevent writers
        }
        sem_post(&mutex);

        // Read data
        printf("Reader read data: %d\n", data);

        // Reader exit section
        sem_wait(&mutex);
        readers_count--;
        if (readers_count == 0) { // Last reader
            sem_post(&write_mutex); // Allow writers
        }
        sem_post(&mutex);

        // Sleep to simulate reading
        sleep(1);
    }
}

void *writer(void *arg) {
    while (1) {
        // Writer entry section
        sem_wait(&write_mutex);

        // Write data
        data++;
        printf("Writer wrote data: %d\n", data);

        // Writer exit section
        sem_post(&write_mutex);

        // Sleep to simulate writing
        sleep(2);
    }
}

