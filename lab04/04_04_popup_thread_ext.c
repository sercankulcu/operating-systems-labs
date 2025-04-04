// Include necessary headers
#include <stdio.h>      // For input/output operations
#include <stdlib.h>     // For exit and EXIT_ constants
#include <pthread.h>    // For thread operations
#include <unistd.h>     // For sleep
#include <string.h>     // For string operations
#include <time.h>       // For timestamp functionality

// Define constants
#define MAX_POPUPS 3    // Maximum number of concurrent popups
#define BUFFER_SIZE 128 // Size of message buffer

// Structure to hold popup parameters
struct popup_params {
    int id;             // Unique identifier for popup
    char message[BUFFER_SIZE]; // Popup message content
    int duration;       // Duration in seconds
};

// Thread function to handle popup display
void *popup_thread(void *arg) {
    // Cast argument to our parameter structure
    struct popup_params *params = (struct popup_params *)arg;
    
    // Get current time for timestamp
    time_t now;
    time(&now);
    
    // Display popup start message with timestamp
    printf("[%.24s] Popup %d: %s\n", ctime(&now), params->id, params->message);
    
    // Simulate popup display duration
    for (int i = params->duration; i > 0; i--) {
        printf("Popup %d: %d seconds remaining\n", params->id, i);
        sleep(1);
    }
    
    // Display popup dismissal
    time(&now);
    printf("[%.24s] Popup %d: Dismissed\n", ctime(&now), params->id);
    
    // Clean up and exit thread
    free(params);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[MAX_POPUPS];    // Array to store thread IDs
    int active_popups = 0;           // Counter for active popups
    
    // Array of sample popup messages
    const char *messages[] = {
        "System Update Available",
        "Low Battery Warning",
        "New Message Received"
    };
    
    // Create multiple popup threads
    for (int i = 0; i < MAX_POPUPS; i++) {
        // Allocate memory for popup parameters
        struct popup_params *params = malloc(sizeof(struct popup_params));
        if (params == NULL) {
            perror("Failed to allocate memory for popup parameters");
            exit(EXIT_FAILURE);
        }
        
        // Initialize popup parameters
        params->id = i + 1;
        snprintf(params->message, BUFFER_SIZE, "%s", messages[i]);
        params->duration = 2 + i; // Varying durations: 2, 3, 4 seconds
        
        // Announce popup creation
        printf("Main thread: Creating popup %d for '%s'\n", 
               params->id, params->message);
        
        // Create thread
        if (pthread_create(&threads[i], NULL, popup_thread, params) != 0) {
            perror("pthread_create failed");
            free(params);
            // Cleanup previously created threads
            for (int j = 0; j < i; j++) {
                pthread_cancel(threads[j]);
            }
            exit(EXIT_FAILURE);
        }
        
        active_popups++;
        sleep(1); // Stagger popup creation
    }
    
    // Main thread continues with other tasks
    printf("Main thread: Performing background tasks...\n");
    
    // Wait for all popup threads to complete
    for (int i = 0; i < active_popups; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join failed");
            // Continue with remaining joins even if one fails
            continue;
        }
        printf("Main thread: Popup %d has completed\n", i + 1);
    }
    
    // Final status message
    time_t now;
    time(&now);
    printf("[%.24s] Main thread: All popups and tasks completed\n", ctime(&now));
    
    return 0;
}
