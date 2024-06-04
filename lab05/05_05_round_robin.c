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
    int n = 5;
    int quantum = 2;

    // Array to store processes
    struct Process processes[n];

    // Input process details
    processes[0].id = 1;
    processes[0].burst = 10;
    processes[0].arrival = 1;
    processes[0].remaining = processes[0].burst;
    processes[0].completed = 0;
    processes[1].id = 2;
    processes[1].burst = 8;
    processes[1].arrival = 2;
    processes[1].remaining = processes[1].burst;
    processes[1].completed = 0;
    processes[2].id = 3;
    processes[2].burst = 6;
    processes[2].arrival = 3;
    processes[2].remaining = processes[2].burst;
    processes[2].completed = 0;
    processes[3].id = 4;
    processes[3].burst = 4;
    processes[3].arrival = 4;
    processes[3].remaining = processes[3].burst;
    processes[3].completed = 0;
    processes[4].id = 5;
    processes[4].burst = 2;
    processes[4].arrival = 5;
    processes[4].remaining = processes[4].burst;
    processes[4].completed = 0;

    // Display processes along with all details
    printf("Process\t\tBurst Time\tArrival Time\n");

    // Calculate total waiting time and turnaround time
    for (int i = 0; i < n; i++) {
        printf("%d\t\t%d\t\t%d\n", processes[i].id, processes[i].burst, processes[i].arrival);
    }

    // Perform Round Robin scheduling
    printf("Sequence of processes execution:\n");
    roundRobinScheduling(processes, n, quantum);

    return 0;
}

