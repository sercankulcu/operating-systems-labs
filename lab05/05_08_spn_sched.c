#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

// Process structure
struct Process {
    int id;         // Process ID
    int burst;      // Burst time
    int arrival;    // Arrival time
    bool completed; // Flag to check if process completed
};

// Function to find the process with the shortest remaining time
int findShortestProcess(struct Process processes[], int n, int currentTime) {
    int shortest = -1;
    int shortestBurst = INT_MAX;

    for (int i = 0; i < n; i++) {
        if (processes[i].arrival <= currentTime && !processes[i].completed && processes[i].burst < shortestBurst) {
            shortest = i;
            shortestBurst = processes[i].burst;
        }
    }

    return shortest;
}

// Function to perform SPN scheduling
void spnScheduling(struct Process processes[], int n) {
    int currentTime = 0;
    int completed = 0;

    printf("Sequence of processes execution:\n");
    while (completed != n) {
        int shortest = findShortestProcess(processes, n, currentTime);

        if (shortest == -1) {
            currentTime++;
        } else {
            // Execute the shortest process
            printf("Process %d is running from time %d to ", processes[shortest].id, currentTime);
            currentTime += processes[shortest].burst;
            printf("%d\n", currentTime);
            processes[shortest].completed = true;
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
    printf("Enter burst time and arrival time for each process:\n");
    for (int i = 0; i < n; i++) {
        printf("Process %d: ", i + 1);
        processes[i].id = i + 1;
        scanf("%d %d", &processes[i].burst, &processes[i].arrival);
        processes[i].completed = false;
    }

    // Perform SPN scheduling
    spnScheduling(processes, n);

    return 0;
}

