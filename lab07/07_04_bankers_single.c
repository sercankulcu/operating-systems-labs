// Include necessary headers
#include <stdio.h>     // For input/output
#include <stdbool.h>   // For boolean type
#include <stdlib.h>    // For exit

// Define constants
#define NUM_PROCESSES 5    // Number of processes
#define NUM_RESOURCES 1    // Number of resource types

// Global resource matrices
int available[NUM_RESOURCES] = {0};  // Available resource instances
int max_claim[NUM_PROCESSES][NUM_RESOURCES] = {  // Maximum resource claims
    {7}, {3}, {9}, {2}, {4}
};
int allocation[NUM_PROCESSES][NUM_RESOURCES] = {  // Currently allocated resources
    {0}, {2}, {3}, {2}, {2}
};
int need[NUM_PROCESSES][NUM_RESOURCES];  // Remaining resource needs

// Function prototypes
bool find_runnable_process(int available[],  
                         int need[][NUM_RESOURCES], bool finished[], int *pid);
void print_system_state();
void run_process(int pid);

// Print the current system state
void print_system_state() {
    printf("\nSystem State:\n");
    printf("Available: ");
    for (int j = 0; j < NUM_RESOURCES; j++) {
        printf("%d ", available[j]);
    }
    printf("\nProcess | Max | Alloc | Need\n");
    for (int i = 0; i < NUM_PROCESSES; i++) {
        printf("P%d      | ", i);
        for (int j = 0; j < NUM_RESOURCES; j++) printf("%d ", max_claim[i][j]);
        printf("| ");
        for (int j = 0; j < NUM_RESOURCES; j++) printf("%d ", allocation[i][j]);
        printf("| ");
        for (int j = 0; j < NUM_RESOURCES; j++) printf("%d ", need[i][j]);
        printf("\n");
    }
}

int main() {
    bool finished[NUM_PROCESSES] = {false};  // Track completed processes
    int run_order[NUM_PROCESSES];           // Store execution order
    int completed = 0;                     // Count of completed processes
    
    // Calculate initial need matrix
    for (int i = 0; i < NUM_PROCESSES; i++) {
        for (int j = 0; j < NUM_RESOURCES; j++) {
            need[i][j] = max_claim[i][j] - allocation[i][j];
        }
    }
    
    // Print initial state
    printf("Initial ");
    print_system_state();
    
    // Run processes one by one
    while (completed < NUM_PROCESSES) {
        int pid;
        if (find_runnable_process(available, need, finished, &pid)) {
            run_order[completed] = pid;  // Record process in run order
            run_process(pid);           // Execute the process
            finished[pid] = true;       // Mark as completed
            completed++;
            printf("After P%d completes: ", pid);
            print_system_state();
        } else {
            printf("\nDeadlock detected: No process can run with current resources\n");
            break;
        }
    }
    
    // Print final execution order if all processes completed
    if (completed == NUM_PROCESSES) {
        printf("\nExecution order of processes: ");
        for (int i = 0; i < NUM_PROCESSES; i++) {
            printf("P%d", run_order[i]);
            if (i < NUM_PROCESSES - 1) printf(" -> ");
        }
        printf("\nAll processes completed successfully\n");
    }
    
    return 0;
}

// Find a process that can run with current resources
bool find_runnable_process(int available[],
                         int need[][NUM_RESOURCES], bool finished[], int *pid) {
    for (int i = 0; i < NUM_PROCESSES; i++) {
        if (!finished[i]) {  // Check only unfinished processes
            bool can_run = true;
            for (int j = 0; j < NUM_RESOURCES; j++) {
                if (need[i][j] > available[j]) {
                    can_run = false;
                    break;
                }
            }
            if (can_run) {
                *pid = i;  // Return the process ID
                return true;
            }
        }
    }
    return false;  // No runnable process found
}

// Simulate running a process and update resources
void run_process(int pid) {
    printf("\nRunning Process P%d\n", pid);
    // Simulate process execution by releasing allocated resources
    for (int j = 0; j < NUM_RESOURCES; j++) {
        available[j] += allocation[pid][j];  // Return allocated resources
        allocation[pid][j] = 0;             // Clear allocation
        need[pid][j] = 0;                  // Clear need (process is done)
    }
}

