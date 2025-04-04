// Include necessary headers
#include <stdio.h>      // For printf
#include <stdlib.h>     // For exit
#include <pthread.h>    // For thread operations
#include <unistd.h>     // For sleep
#include <stdint.h>     // For intptr_t

// Define constants
#define NUM_PHILOSOPHERS 5    // Number of philosophers
#define MAX_EAT_COUNT 3       // Number of times each philosopher eats

// Global variables
pthread_mutex_t forks[NUM_PHILOSOPHERS];  // Mutexes representing forks

// Function prototypes
void *philosopher(void *arg);
void grab_forks(int philosopher_id);
void release_forks(int philosopher_id);
int left_fork_index(int philosopher_id);
int right_fork_index(int philosopher_id);

int main() {
    pthread_t philosophers[NUM_PHILOSOPHERS];  // Array of philosopher threads
    int philosopher_ids[NUM_PHILOSOPHERS];    // Array to store IDs
    
    // Initialize mutexes (forks)
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        if (pthread_mutex_init(&forks[i], NULL) != 0) {
            perror("Mutex init failed");
            exit(EXIT_FAILURE);
        }
    }
    
    // Create philosopher threads with IDs
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        philosopher_ids[i] = i;
        if (pthread_create(&philosophers[i], NULL, philosopher, 
                          (void *)(intptr_t)&philosopher_ids[i]) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }
    
    // Wait for all philosophers to finish
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        if (pthread_join(philosophers[i], NULL) != 0) {
            perror("Thread join failed");
            // Continue with remaining joins
            continue;
        }
        printf("Philosopher %d has finished dining\n", i);
    }
    
    // Clean up mutexes
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        if (pthread_mutex_destroy(&forks[i]) != 0) {
            perror("Mutex destroy failed");
            exit(EXIT_FAILURE);
        }
    }
    
    printf("Dinner is over!\n");
    return 0;
}

// Philosopher thread function
void *philosopher(void *arg) {
    int *id_ptr = (int *)arg;
    int philosopher_id = *id_ptr;    // Get philosopher ID
    int eat_count = 0;              // Track eating instances
    
    while (eat_count < MAX_EAT_COUNT) {
        // Thinking phase
        printf("Philosopher %d is thinking\n", philosopher_id);
        sleep(rand() % 2);  // Random thinking time
        
        // Eating phase with deadlock prevention
        grab_forks(philosopher_id);
        
        printf("Philosopher %d is eating (meal %d/%d)\n", 
               philosopher_id, eat_count + 1, MAX_EAT_COUNT);
        sleep(1);  // Simulate eating time
        
        release_forks(philosopher_id);
        
        eat_count++;
    }
    
    printf("Philosopher %d is full and leaving\n", philosopher_id);
    pthread_exit(NULL);
}

// Grab forks with resource hierarchy solution to prevent deadlock
void grab_forks(int philosopher_id) {
    int left = left_fork_index(philosopher_id);
    int right = right_fork_index(philosopher_id);
    
    // Order locks to prevent deadlock: always lock lower-numbered fork first
    if (left < right) {
        pthread_mutex_lock(&forks[left]);
        printf("Philosopher %d picked up fork %d\n", philosopher_id, left);
        pthread_mutex_lock(&forks[right]);
        printf("Philosopher %d picked up fork %d\n", philosopher_id, right);
    } else {
        pthread_mutex_lock(&forks[right]);
        printf("Philosopher %d picked up fork %d\n", philosopher_id, right);
        pthread_mutex_lock(&forks[left]);
        printf("Philosopher %d picked up fork %d\n", philosopher_id, left);
    }
}

// Release both forks
void release_forks(int philosopher_id) {
    int left = left_fork_index(philosopher_id);
    int right = right_fork_index(philosopher_id);
    
    pthread_mutex_unlock(&forks[left]);
    printf("Philosopher %d released fork %d\n", philosopher_id, left);
    pthread_mutex_unlock(&forks[right]);
    printf("Philosopher %d released fork %d\n", philosopher_id, right);
}

// Calculate left fork index
int left_fork_index(int philosopher_id) {
    return philosopher_id;
}

// Calculate right fork index (circular table)
int right_fork_index(int philosopher_id) {
    return (philosopher_id + 1) % NUM_PHILOSOPHERS;
}
