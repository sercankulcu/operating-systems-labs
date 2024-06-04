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
    int n = 5;

    // Array to store processes
    struct Process processes[n];

    // Input process details
    processes[0].id = 1;
    processes[0].burst = 10;
    processes[0].arrival = 1;
    processes[0].completed = 0;
    processes[1].id = 2;
    processes[1].burst = 8;
    processes[1].arrival = 2;
    processes[1].completed = 0;
    processes[2].id = 3;
    processes[2].burst = 6;
    processes[2].arrival = 3;
    processes[2].completed = 0;
    processes[3].id = 4;
    processes[3].burst = 4;
    processes[3].arrival = 4;
    processes[3].completed = 0;
    processes[4].id = 5;
    processes[4].burst = 2;
    processes[4].arrival = 5;
    processes[4].completed = 0;

    // Display processes along with all details
    printf("Process\t\tBurst Time\tArrival Time\n");

    // Calculate total waiting time and turnaround time
    for (int i = 0; i < n; i++) {
        printf("%d\t\t%d\t\t%d\n", processes[i].id, processes[i].burst, processes[i].arrival);
    }

    // Perform SPN scheduling
    spnScheduling(processes, n);

    return 0;
}

