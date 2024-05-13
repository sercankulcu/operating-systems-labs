#include <stdio.h>
#include <stdbool.h>

#define NUM_PROCESSES 5
#define NUM_RESOURCES 1

int available[NUM_RESOURCES] = {10}; // Available instances of the resource
int max_claim[NUM_PROCESSES][NUM_RESOURCES] = {
    {7},
    {3},
    {9},
    {2},
    {4}
};
int allocation[NUM_PROCESSES][NUM_RESOURCES] = {
    {0},
    {2},
    {3},
    {2},
    {0}
};
int need[NUM_PROCESSES][NUM_RESOURCES];

bool is_safe_state(int available[], int allocation[][NUM_RESOURCES], int need[][NUM_RESOURCES]);
bool request_resources(int pid, int request[]);

int main() {
    // Calculate need matrix
    for (int i = 0; i < NUM_PROCESSES; i++) {
        for (int j = 0; j < NUM_RESOURCES; j++) {
            need[i][j] = max_claim[i][j] - allocation[i][j];
        }
    }

    // Run Banker's algorithm
    if (is_safe_state(available, allocation, need)) {
        printf("System is in a safe state.\n");

        // Example request
        int pid = 1;
        int request[NUM_RESOURCES] = {1};
        if (request_resources(pid, request)) {
            printf("Request from Process %d granted.\n", pid);
        } else {
            printf("Request from Process %d denied (unsafe state).\n", pid);
        }
    } else {
        printf("System is in an unsafe state.\n");
    }

    return 0;
}

bool is_safe_state(int available[], int allocation[][NUM_RESOURCES], int need[][NUM_RESOURCES]) {
    bool finish[NUM_PROCESSES] = {false};
    int work[NUM_RESOURCES];
    
    // Initialize work array
    for (int i = 0; i < NUM_RESOURCES; i++) {
        work[i] = available[i];
    }

    int count = 0;
    while (count < NUM_PROCESSES) {
        bool found = false;
        for (int i = 0; i < NUM_PROCESSES; i++) {
            if (!finish[i]) {
                bool can_allocate = true;
                for (int j = 0; j < NUM_RESOURCES; j++) {
                    if (need[i][j] > work[j]) {
                        can_allocate = false;
                        break;
                    }
                }
                if (can_allocate) {
                    // Process can be allocated resources
                    for (int j = 0; j < NUM_RESOURCES; j++) {
                        work[j] += allocation[i][j];
                    }
                    finish[i] = true;
                    found = true;
                    count++;
                }
            }
        }
        if (!found) {
            // No process found that can be allocated resources
            break;
        }
    }

    // Check if all processes are finished
    for (int i = 0; i < NUM_PROCESSES; i++) {
        if (!finish[i]) {
            return false; // System is in an unsafe state
        }
    }

    return true; // System is in a safe state
}

bool request_resources(int pid, int request[]) {
    // Check if request is within need
    for (int i = 0; i < NUM_RESOURCES; i++) {
        if (request[i] > need[pid][i]) {
            return false; // Request exceeds need
        }
    }

    // Check if request is within available
    for (int i = 0; i < NUM_RESOURCES; i++) {
        if (request[i] > available[i]) {
            return false; // Insufficient resources available
        }
    }

    // Try to allocate resources
    for (int i = 0; i < NUM_RESOURCES; i++) {
        available[i] -= request[i];
        allocation[pid][i] += request[i];
        need[pid][i] -= request[i];
    }

    // Check if state is still safe
    if (!is_safe_state(available, allocation, need)) {
        // Restore state if unsafe
        for (int i = 0; i < NUM_RESOURCES; i++) {
            available[i] += request[i];
            allocation[pid][i] -= request[i];
            need[pid][i] += request[i];
        }
        return false; // Unsafe state
    }

    return true; // Safe state, request granted
}

