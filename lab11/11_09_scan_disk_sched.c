#include <stdio.h>
#include <stdlib.h>

#define MAX_REQUESTS 100

// Function to simulate SCAN disk arm scheduling
void scan_disk_scheduling(int requests[], int num_requests, int initial_position, int direction) {
    int current_position = initial_position;
    int total_head_movement = 0;
    int visited[MAX_REQUESTS] = {0}; // Array to keep track of visited requests

    printf("SCAN Disk Arm Scheduling\n");

    // Process requests in the specified direction
    while (1) {
        // Scan in the forward direction
        if (direction == 1) {
            for (int i = 0; i < num_requests; i++) {
                if (requests[i] >= current_position && !visited[i]) {
                    int distance = abs(requests[i] - current_position);
                    total_head_movement += distance;
                    current_position = requests[i];
                    visited[i] = 1;

                    printf("Moved to track %d, Head movement: %d\n", current_position, distance);
                }
            }
            // Change direction when reaching the end
            direction = -1;
        } 
        // Scan in the reverse direction
        else {
            for (int i = num_requests - 1; i >= 0; i--) {
                if (requests[i] <= current_position && !visited[i]) {
                    int distance = abs(requests[i] - current_position);
                    total_head_movement += distance;
                    current_position = requests[i];
                    visited[i] = 1;

                    printf("Moved to track %d, Head movement: %d\n", current_position, distance);
                }
            }
            // Change direction when reaching the end
            direction = 1;
        }

        // Break if all requests have been serviced
        int all_visited = 1;
        for (int i = 0; i < num_requests; i++) {
            if (!visited[i]) {
                all_visited = 0;
                break;
            }
        }
        if (all_visited) {
            break;
        }
    }

    printf("Total Head Movement: %d\n", total_head_movement);
}

int main() {

    int requests[MAX_REQUESTS] = { 98, 183, 37, 122, 14, 124, 65, 67};
    int num_requests = 8;
    int initial_position = 53;

    // Simulate SCAN disk arm scheduling
    scan_disk_scheduling(requests, num_requests, initial_position, 1);

    return 0;
}

