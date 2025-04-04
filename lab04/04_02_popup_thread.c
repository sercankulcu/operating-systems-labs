#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *popup_thread() {
    // Thread function to handle pop-up event
    printf("Pop-up thread: Pop-up event detected!\n");
    // Simulating pop-up display for 3 seconds
    sleep(3);
    printf("Pop-up thread: Pop-up dismissed.\n");
    pthread_exit(NULL);
}

int main() {
    pthread_t thread;

    // Simulating the occurrence of a pop-up event
    printf("Main thread: Pop-up event detected. Creating pop-up thread...\n");
    if (pthread_create(&thread, NULL, popup_thread, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    // Main thread continues executing other tasks while pop-up is displayed
    printf("Main thread: Continuing other tasks while pop-up is displayed...\n");
    sleep(5); // Simulating other tasks being performed

    // Wait for the pop-up thread to finish
    if (pthread_join(thread, NULL) != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    printf("Main thread: All tasks completed.\n");

    return 0;
}

