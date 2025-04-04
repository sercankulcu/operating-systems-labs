// Include necessary headers
#include <stdio.h>     // For input/output
#include <stdbool.h>   // For boolean type
#include <stdlib.h>    // For exit

// Define constants
#define NUM_PROCESSES 4    // Number of processes (P0 to P3)
#define NUM_RESOURCES 3    // Number of resources (R0 to R2)

// Global matrices
int allocation[NUM_PROCESSES][NUM_RESOURCES] = {  // Current allocation
    {1, 0, 0},  // P0 holds R0
    {0, 1, 0},  // P1 holds R1
    {0, 0, 1},  // P2 holds R2
    {0, 0, 0}   // P3 holds nothing
};

int request[NUM_PROCESSES][NUM_RESOURCES] = {     // Current requests
    {0, 1, 0},  // P0 requests R1
    {0, 0, 1},  // P1 requests R2
    {1, 0, 0},  // P2 requests R0
    {0, 0, 1}   // P3 requests R2
};

// Function prototypes
bool detect_deadlock(int allocation[][NUM_RESOURCES], int request[][NUM_RESOURCES]);
void print_system_state();
bool is_cycle_present(int process, bool visited[], bool rec_stack[], 
                     int allocation[][NUM_RESOURCES], int request[][NUM_RESOURCES]);

int main() {
    // Print initial system state
    print_system_state();
    
    // Check for deadlock
    if (detect_deadlock(allocation, request)) {
        printf("\nDeadlock detected in the system!\n");
    } else {
        printf("\nNo deadlock detected in the system.\n");
    }
    
    return 0;
}

// Detect deadlock using cycle detection in resource allocation graph
bool detect_deadlock(int allocation[][NUM_RESOURCES], int request[][NUM_RESOURCES]) {
    bool visited[NUM_PROCESSES] = {false};    // Track visited processes
    bool rec_stack[NUM_PROCESSES] = {false};  // Track processes in recursion stack
    
    // Check each process for potential cycle
    for (int i = 0; i < NUM_PROCESSES; i++) {
        if (!visited[i]) {
            if (is_cycle_present(i, visited, rec_stack, allocation, request)) {
                return true;  // Deadlock found
            }
        }
    }
    return false;  // No deadlock
}

// Recursive function to detect cycle in resource allocation graph
bool is_cycle_present(int process, bool visited[], bool rec_stack[], 
                     int allocation[][NUM_RESOURCES], int request[][NUM_RESOURCES]) {
    visited[process] = true;    // Mark process as visited
    rec_stack[process] = true;  // Add to recursion stack
    
    // Check all resources this process requests
    for (int r = 0; r < NUM_RESOURCES; r++) {
        if (request[process][r] > 0) {  // Process requests this resource
            // Find which process holds this resource
            for (int p = 0; p < NUM_PROCESSES; p++) {
                if (allocation[p][r] > 0) {  // Process p holds resource r
                    if (!visited[p]) {
                        // Recursively check next process
                        if (is_cycle_present(p, visited, rec_stack, allocation, request)) {
                            return true;
                        }
                    } else if (rec_stack[p]) {
                        // Cycle detected: process p is already in recursion stack
                        printf("Cycle detected: P%d -> P%d\n", process, p);
                        return true;
                    }
                }
            }
        }
    }
    
    rec_stack[process] = false;  // Remove from recursion stack
    return false;  // No cycle from this process
}

// Print current system state
void print_system_state() {
    printf("System State:\n");
    printf("Process | Allocation | Request\n");
    for (int i = 0; i < NUM_PROCESSES; i++) {
        printf("P%d      | ", i);
        for (int j = 0; j < NUM_RESOURCES; j++) {
            printf("%d ", allocation[i][j]);
        }
        printf("| ");
        for (int j = 0; j < NUM_RESOURCES; j++) {
            printf("%d ", request[i][j]);
        }
        printf("\n");
    }
}
