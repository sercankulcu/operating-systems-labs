#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h> // Include this header for intptr_t

#define NUM_PHILOSOPHERS 5

pthread_mutex_t forks[NUM_PHILOSOPHERS];

void *philosopher(void *arg);
void grab_forks(int philosopher_id);
void release_forks(int philosopher_id);
int left_fork_index(int philosopher_id);
int right_fork_index(int philosopher_id);

int main() {
    pthread_t philosophers[NUM_PHILOSOPHERS];

    // Initialize mutexes (forks)
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }

    // Create philosopher threads
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_create(&philosophers[i], NULL, philosopher, (void *)(intptr_t)i);
    }

    // Join philosopher threads
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    // Destroy mutexes
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&forks[i]);
    }

    return 0;
}

void *philosopher(void *arg) {
    int philosopher_id = (intptr_t)arg;
    int eat_count = 0;

    while (eat_count < 3) { // Philosophers eat 3 times
        printf("Philosopher %d is thinking.\n", philosopher_id);

        // Try to grab forks
        grab_forks(philosopher_id);

        printf("Philosopher %d is eating.\n", philosopher_id);
        sleep(1); // Simulate eating

        // Release forks
        release_forks(philosopher_id);

        eat_count++;
    }

    pthread_exit(NULL);
}

void grab_forks(int philosopher_id) {
    int left_fork = left_fork_index(philosopher_id);
    int right_fork = right_fork_index(philosopher_id);

    // Lock the mutexes for left and right forks
    pthread_mutex_lock(&forks[left_fork]);
    pthread_mutex_lock(&forks[right_fork]);
}

void release_forks(int philosopher_id) {
    int left_fork = left_fork_index(philosopher_id);
    int right_fork = right_fork_index(philosopher_id);

    // Unlock the mutexes for left and right forks
    pthread_mutex_unlock(&forks[left_fork]);
    pthread_mutex_unlock(&forks[right_fork]);
}

int left_fork_index(int philosopher_id) {
    return philosopher_id;
}

int right_fork_index(int philosopher_id) {
    return (philosopher_id + 1) % NUM_PHILOSOPHERS;
}

