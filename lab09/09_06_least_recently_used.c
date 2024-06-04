#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Structure to represent a page
typedef struct {
    int pageNumber;
    int timestamp;
} Page;

// Function to initialize the page table
void initializePageTable(Page pageTable[], int numPages) {
    for (int i = 0; i < numPages; i++) {
        pageTable[i].pageNumber = -1;
        pageTable[i].timestamp = 0;
    }
}

// Function to find the index of the least recently used page
int findLeastRecentlyUsed(Page pageTable[], int numPages) {
    int minTimestamp = pageTable[0].timestamp;
    int lruIndex = 0;

    for (int i = 1; i < numPages; i++) {
        if (pageTable[i].timestamp < minTimestamp) {
            minTimestamp = pageTable[i].timestamp;
            lruIndex = i;
        }
    }

    return lruIndex;
}

// Function to perform Least Recently Used (LRU) page replacement
void lruPageReplacement(int referenceString[], int numReferences, int numFrames) {
    int pageFaults = 0;
    Page *pageTable = (Page *)malloc(numFrames * sizeof(Page));

    // Initialize page table
    initializePageTable(pageTable, numFrames);

    for (int i = 0; i < numReferences; i++) {
        int pageNumber = referenceString[i];
        bool pageFault = true;

        // Check if page is already in memory
        for (int j = 0; j < numFrames; j++) {
            if (pageTable[j].pageNumber == pageNumber) {
                pageTable[j].timestamp = i + 1; // Update timestamp
                pageFault = false;
                break;
            }
        }

        // If page is not in memory, perform page replacement
        if (pageFault) {
            pageFaults++;
            int lruIndex = findLeastRecentlyUsed(pageTable, numFrames);
            pageTable[lruIndex].pageNumber = pageNumber;
            pageTable[lruIndex].timestamp = i + 1; // Update timestamp
        }

        // Print memory status after each memory access
        printf("Memory after accessing page %d: ", pageNumber);
        for (int j = 0; j < numFrames; j++) {
            if (pageTable[j].pageNumber != -1)
                printf("%d ", pageTable[j].pageNumber);
            else
                printf("_ ");
        }
        printf("\n");
    }

    printf("Total page faults: %d\n", pageFaults);

    free(pageTable);
}

int main() {
    int referenceString[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
    int numReferences = sizeof(referenceString) / sizeof(referenceString[0]);
    int numFrames = 3; // Number of frames in memory

    printf("LRU Page Replacement Simulation:\n");
    lruPageReplacement(referenceString, numReferences, numFrames);

    return 0;
}

