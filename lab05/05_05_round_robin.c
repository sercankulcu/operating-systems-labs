#include <stdio.h>

// Process structure
struct Process {
    int id;         // Process ID
    int burst;      // Burst time
    int remaining;  // Remaining burst time
    int arrival;    // Arrival time
    int completed;  // Flag to check if process completed
};

// Function to perform Round Robin scheduling
void roundRobinScheduling(struct Process processes[], int n, int quantum) {
    int currentTime = 0;
    int completed = 0;

    while (completed != n) {
        for (int i = 0; i < n; i++) {
            if (processes[i].remaining > 0) {
                // Execute the process for the quantum or its remaining time, whichever is smaller
                if (processes[i].remaining <= quantum) {
                    currentTime += processes[i].remaining;
                    processes[i].remaining = 0;
                    processes[i].completed = 1;
                    printf("Process %d is running from time %d to %d\n", processes[i].id, currentTime - processes[i].burst, currentTime);
                    completed++;
                } else {
                    currentTime += quantum;
                    processes[i].remaining -= quantum;
                    printf("Process %d is running from time %d to %d\n", processes[i].id, currentTime - quantum, currentTime);
                }
            }
        }
    }
}

int main() {
    // Number of processes and time quantum
    int n, quantum;

    printf("Enter the number of processes: ");
    scanf("%d", &n);

    printf("Enter time quantum: ");
    scanf("%d", &quantum);

    // Array to store processes
    struct Process processes[n];

    // Input process details
    printf("Enter burst time and arrival time for each process:\n");
    for (int i = 0; i < n; i++) {
        printf("Process %d: ", i + 1);
        processes[i].id = i + 1;
        scanf("%d %d", &processes[i].burst, &processes[i].arrival);
        processes[i].remaining = processes[i].burst;
        processes[i].completed = 0;
    }

    // Perform Round Robin scheduling
    printf("Sequence of processes execution:\n");
    roundRobinScheduling(processes, n, quantum);

    return 0;
}

