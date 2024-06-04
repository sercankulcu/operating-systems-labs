#include <stdio.h>

// Process structure
struct Process {
    int id;         // Process ID
    int burst;      // Burst time
    int executed;   // Time already executed
    int guaranteed; // Guaranteed CPU time
};

// Function to perform Guaranteed Scheduling using GMR algorithm
void guaranteedScheduling(struct Process processes[], int n) {
    int currentTime = 0;
    int completed = 0;

    printf("Sequence of processes execution:\n");
    while (completed != n) {
        // Execute each process for its guaranteed time within the interval
        for (int i = 0; i < n; i++) {
            if (processes[i].executed < processes[i].burst) {
                int remainingTime = processes[i].burst - processes[i].executed;
                int executionTime = processes[i].guaranteed < remainingTime ? processes[i].guaranteed : remainingTime;
                printf("Process %d is running from time %d to ", processes[i].id, currentTime);
                currentTime += executionTime;
                processes[i].executed += executionTime;
                printf("%d\n", currentTime);
                if (processes[i].executed - processes[i].burst == 0) {
                    completed++;
                    printf("Process %d terminated\n", processes[i].id);
                }
            }
        }
    }
}

int main() {
    // Number of processes and interval
    int n = 5;

    // Array to store processes
    struct Process processes[n];

    // Input process details
    processes[0].id = 1;
    processes[0].burst = 10;
    processes[0].guaranteed = 1;
    processes[0].executed = 0;
    processes[1].id = 2;
    processes[1].burst = 8;
    processes[1].guaranteed = 2;
    processes[1].executed = 0;
    processes[2].id = 3;
    processes[2].burst = 6;
    processes[2].guaranteed = 3;
    processes[2].executed = 0;
    processes[3].id = 4;
    processes[3].burst = 4;
    processes[3].guaranteed = 4;
    processes[3].executed = 0;
    processes[4].id = 5;
    processes[4].burst = 2;
    processes[4].guaranteed = 5;
    processes[4].executed = 0;

    // Display processes along with all details
    printf("Process\t\tBurst Time\tGuaranteed Time\n");

    // Calculate total waiting time and turnaround time
    for (int i = 0; i < n; i++) {
        printf("%d\t\t%d\t\t%d\n", processes[i].id, processes[i].burst, processes[i].guaranteed);
    }

    // Perform Guaranteed Scheduling using GMR algorithm
    guaranteedScheduling(processes, n);

    return 0;
}

