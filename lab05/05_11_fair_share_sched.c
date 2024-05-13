#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 100

// Process structure
struct Process {
    int id;         // Process ID
    int weight;     // Weight representing fair share
    int executed;   // Time already executed
};

// Function to perform fair-share scheduling
void fairShareScheduling(struct Process processes[], int n, int totalTime) {
    int remainingTime[MAX_PROCESSES];
    float completionRatio[MAX_PROCESSES];
    int completed = 0;

    // Initialize remaining time for each process
    for (int i = 0; i < n; i++)
        remainingTime[i] = processes[i].weight;

    printf("Sequence of processes execution:\n");
    // Continue until all processes are completed or total time is reached
    for (int currentTime = 0; currentTime < totalTime && completed != n; currentTime++) {
        float maxRatio = 0;
        int maxIndex = -1;
        // Find the process with the maximum completion ratio
        for (int i = 0; i < n; i++) {
            completionRatio[i] = (float)remainingTime[i] / processes[i].weight;
            if (!processes[i].executed && completionRatio[i] > maxRatio) {
                maxRatio = completionRatio[i];
                maxIndex = i;
            }
        }
        // Execute the process with the maximum completion ratio
        if (maxIndex != -1) {
            printf("Process %d is running from time %d to ", processes[maxIndex].id, currentTime);
            currentTime += 1;
            printf("%d\n", currentTime);
            processes[maxIndex].executed = 1;
            remainingTime[maxIndex]--;
            if (remainingTime[maxIndex] == 0)
                completed++;
        }
    }
}

int main() {
    // Number of processes and total time
    int n, totalTime;

    printf("Enter the number of processes: ");
    scanf("%d", &n);

    printf("Enter total time: ");
    scanf("%d", &totalTime);

    // Array to store processes
    struct Process processes[MAX_PROCESSES];

    // Input process details
    printf("Enter weight for each process:\n");
    for (int i = 0; i < n; i++) {
        printf("Process %d: ", i + 1);
        processes[i].id = i + 1;
        scanf("%d", &processes[i].weight);
        processes[i].executed = 0;
    }

    // Perform fair-share scheduling
    fairShareScheduling(processes, n, totalTime);

    return 0;
}

