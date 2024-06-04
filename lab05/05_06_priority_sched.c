#include <stdio.h>

// Process structure
struct Process {
    int id;         // Process ID
    int burst;      // Burst time
    int priority;   // Priority
    int completed;  // Flag to check if process completed
};

// Function to perform Priority Scheduling
void priorityScheduling(struct Process processes[], int n) {
    int currentTime = 0;
    int completed = 0;
    int highestPriority;

    while (completed != n) {
        highestPriority = -1;
        int highestPriorityIndex = -1;

        // Find the process with the highest priority among arrived processes
        for (int i = 0; i < n; i++) {
            if (!processes[i].completed && processes[i].priority > highestPriority && processes[i].burst > 0) {
                highestPriority = processes[i].priority;
                highestPriorityIndex = i;
            }
        }

        if (highestPriorityIndex == -1) {
            currentTime++;
        } else {
            // Execute the process with highest priority
            printf("Process %d is running from time %d to ", processes[highestPriorityIndex].id, currentTime);
            currentTime += processes[highestPriorityIndex].burst;
            printf("%d\n", currentTime);
            processes[highestPriorityIndex].completed = 1;
            completed++;
        }
    }
}

int main() {
    // Number of processes
    int n = 5;

    // Array to store processes
    struct Process processes[n];

    // Input process details
    processes[0].id = 1;
    processes[0].burst = 10;
    processes[0].priority = 1;
    processes[0].completed = 0;
    processes[1].id = 2;
    processes[1].burst = 8;
    processes[1].priority = 2;
    processes[1].completed = 0;
    processes[2].id = 3;
    processes[2].burst = 6;
    processes[2].priority = 5;
    processes[2].completed = 0;
    processes[3].id = 4;
    processes[3].burst = 4;
    processes[3].priority = 3;
    processes[3].completed = 0;
    processes[4].id = 5;
    processes[4].burst = 2;
    processes[4].priority = 4;
    processes[4].completed = 0;

    // Display processes along with all details
    printf("Process\t\tBurst Time\tPriority\n");

    // Calculate total waiting time and turnaround time
    for (int i = 0; i < n; i++) {
        printf("%d\t\t%d\t\t%d\n", processes[i].id, processes[i].burst, processes[i].priority);
    }

    // Perform Priority Scheduling
    printf("Sequence of processes execution:\n");
    priorityScheduling(processes, n);

    return 0;
}

