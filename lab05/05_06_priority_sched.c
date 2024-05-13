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
    int n;

    printf("Enter the number of processes: ");
    scanf("%d", &n);

    // Array to store processes
    struct Process processes[n];

    // Input process details
    printf("Enter burst time and priority for each process:\n");
    for (int i = 0; i < n; i++) {
        printf("Process %d: ", i + 1);
        processes[i].id = i + 1;
        scanf("%d %d", &processes[i].burst, &processes[i].priority);
        processes[i].completed = 0;
    }

    // Perform Priority Scheduling
    printf("Sequence of processes execution:\n");
    priorityScheduling(processes, n);

    return 0;
}

