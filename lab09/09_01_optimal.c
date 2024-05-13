#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// Function to find the index of the page that will not be used for the longest period of time in the future
int findOptimalPage(int pages[], int frames[], int n, int current) {
    int index = -1, farthest = current;
    for (int i = 0; i < n; i++) {
        int j;
        for (j = current; j < n; j++) {
            if (frames[i] == pages[j]) {
                if (j > farthest) {
                    farthest = j;
                    index = i;
                }
                break;
            }
        }
        if (j == n)
            return i;
    }
    return (index == -1) ? 0 : index;
}

// Function to perform optimal page replacement
void optimalPageReplacement(int pages[], int n, int capacity) {
    int frames[capacity], pageFaults = 0;

    for (int i = 0; i < capacity; i++) {
        frames[i] = -1; // Initialize frames as empty
    }

    for (int i = 0; i < n; i++) {
        bool isPageFault = true;

        // Check if page is already in frames
        for (int j = 0; j < capacity; j++) {
            if (frames[j] == pages[i]) {
                isPageFault = false;
                break;
            }
        }

        // If page is not in frames, replace page with optimal choice
        if (isPageFault) {
            int index = findOptimalPage(pages, frames, n, i + 1);
            frames[index] = pages[i];
            pageFaults++;
        }

        // Print frames after each memory access
        printf("Memory after accessing page %d: ", pages[i]);
        for (int j = 0; j < capacity; j++) {
            if (frames[j] != -1)
                printf("%d ", frames[j]);
            else
                printf("_ ");
        }
        printf("\n");
    }

    printf("Total page faults: %d\n", pageFaults);
}

int main() {
    int referenceString[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2};
    int n = sizeof(referenceString) / sizeof(referenceString[0]);
    int capacity = 4; // Number of frames in memory

    printf("Optimal Page Replacement Simulation:\n");
    optimalPageReplacement(referenceString, n, capacity);

    return 0;
}

/*findOptimalPage() function determines the index of the page that will not be used for the longest period of time in the future based on the given reference string.
 * */
