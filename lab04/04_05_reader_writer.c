// Include necessary headers
#include <stdio.h>      // For input/output
#include <pthread.h>    // For thread operations
#include <semaphore.h>  // For semaphore operations
#include <unistd.h>     // For sleep
#include <stdlib.h>     // For rand and exit

// Define constants
#define NUM_READERS 3   // Number of reader threads
#define NUM_WRITERS 2   // Number of writer threads
#define MAX_VALUE 100   // Maximum value for shared data

// Global variables
int shared_data = 0;           // Shared resource that readers and writers access
int reader_count = 0;         // Counter for active readers
sem_t mutex;                  // Semaphore for mutual exclusion of reader_count
sem_t rw_mutex;              // Semaphore for read/write access to shared_data

// Reader thread function
void *reader(void *arg) {
    int reader_id = *((int *)arg);  // Get reader ID from argument
    
    while (1) {  // Infinite loop to simulate continuous reading
        // Entry section for readers
        sem_wait(&mutex);           // Lock mutex to update reader_count
        reader_count++;            // Increment active reader count
        if (reader_count == 1) {   // If first reader
            sem_wait(&rw_mutex);  // Lock shared resource from writers
        }
        sem_post(&mutex);          // Release mutex
        
        // Critical section: reading shared data
        printf("Reader %d reads value: %d\n", reader_id, shared_data);
        
        // Exit section for readers
        sem_wait(&mutex);          // Lock mutex to update reader_count
        reader_count--;           // Decrement active reader count
        if (reader_count == 0) {   // If last reader
            sem_post(&rw_mutex);  // Release shared resource
        }
        sem_post(&mutex);          // Release mutex
        
        // Simulate some processing time
        sleep(rand() % 3);         // Random delay between 0-2 seconds
    }
    return NULL;
}

// Writer thread function
void *writer(void *arg) {
    int writer_id = *((int *)arg);  // Get writer ID from argument
    
    while (1) {  // Infinite loop to simulate continuous writing
        // Entry section: acquire exclusive access
        sem_wait(&rw_mutex);       // Lock shared resource
        
        // Critical section: writing to shared data
        shared_data = rand() % MAX_VALUE;  // Generate new random value
        printf("Writer %d wrote value: %d\n", writer_id, shared_data);
        
        // Exit section: release access
        sem_post(&rw_mutex);       // Release shared resource
        
        // Simulate some processing time
        sleep(rand() % 2);         // Random delay between 0-1 seconds
    }
    return NULL;
}

int main() {
    pthread_t readers[NUM_READERS];    // Array for reader threads
    pthread_t writers[NUM_WRITERS];    // Array for writer threads
    int reader_ids[NUM_READERS];      // Array to store reader IDs
    int writer_ids[NUM_WRITERS];      // Array to store writer IDs
    
    // Initialize semaphores
    if (sem_init(&mutex, 0, 1) == -1) {    // Binary semaphore for reader_count
        perror("Failed to initialize mutex semaphore");
        exit(1);
    }
    if (sem_init(&rw_mutex, 0, 1) == -1) { // Binary semaphore for shared_data
        perror("Failed to initialize rw_mutex semaphore");
        exit(1);
    }
    
    // Create reader threads
    for (int i = 0; i < NUM_READERS; i++) {
        reader_ids[i] = i + 1;
        if (pthread_create(&readers[i], NULL, reader, &reader_ids[i]) != 0) {
            perror("Failed to create reader thread");
            exit(1);
        }
    }
    
    // Create writer threads
    for (int i = 0; i < NUM_WRITERS; i++) {
        writer_ids[i] = i + 1;
        if (pthread_create(&writers[i], NULL, writer, &writer_ids[i]) != 0) {
            perror("Failed to create writer thread");
            exit(1);
        }
    }
    
    // Let the program run for a while to demonstrate
    sleep(10);
    
    // Note: In a real application, you'd want to properly clean up
    // threads and semaphores. This example runs briefly then exits.
    
    printf("Main thread: Simulation complete\n");
    
    // Cleanup (would normally include pthread_cancel and sem_destroy)
    exit(0);
}
