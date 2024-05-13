#include <stdio.h>

// Process structure
struct Process {
    int id;         // Process ID
    int burst;      // Burst time
    int executed;   // Time already executed
    int guaranteed; // Guaranteed CPU time
};

// Function to perform Guaranteed Scheduling using GMR algorithm
void guaranteedScheduling(struct Process processes[], int n, int interval) {
    int currentTime = 0;
    int completed = 0;

    printf("Sequence of processes execution:\n");
    while (completed != n) {
        // Execute each process for its guaranteed time within the interval
        for (int i = 0; i < n; i++) {
            if (processes[i].burst > 0 && processes[i].executed < processes[i].guaranteed) {
                int remainingTime = processes[i].guaranteed - processes[i].executed;
                int executionTime = remainingTime < interval ? remainingTime : interval;
                printf("Process %d is running from time %d to ", processes[i].id, currentTime);
                currentTime += executionTime;
                processes[i].executed += executionTime;
                processes[i].burst -= executionTime;
                printf("%d\n", currentTime);
                if (processes[i].burst == 0)
                    completed++;
            }
        }
    }
}

int main() {
    // Number of processes and interval
    int n, interval;

    printf("Enter the number of processes: ");
    scanf("%d", &n);

    printf("Enter the interval: ");
    scanf("%d", &interval);

    // Array to store processes
    struct Process processes[n];

    // Input process details
    printf("Enter burst time and guaranteed CPU time for each process:\n");
    for (int i = 0; i < n; i++) {
        printf("Process %d: ", i + 1);
        processes[i].id = i + 1;
        scanf("%d %d", &processes[i].burst, &processes[i].guaranteed);
        processes[i].executed = 0;
    }

    // Perform Guaranteed Scheduling using GMR algorithm
    guaranteedScheduling(processes, n, interval);

    return 0;
}

