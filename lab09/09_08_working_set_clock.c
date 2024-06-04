#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Page table entry structure
typedef struct {
    int pageNumber; // Page number
    bool referenced; // Reference bit
    bool inWorkingSet; // Flag indicating whether the page is in the working set or not
} PageTableEntry;

// Function to initialize the page table
void initializePageTable(PageTableEntry pageTable[], int numPages) {
    for (int i = 0; i < numPages; i++) {
        pageTable[i].pageNumber = -1;
        pageTable[i].referenced = false;
        pageTable[i].inWorkingSet = false;
    }
}

// Function to find the index of the least recently used page
int findLeastRecentlyUsed(PageTableEntry pageTable[], int numPages) {
    int minTimestamp = pageTable[0].referenced;
    int lruIndex = 0;

    for (int i = 1; i < numPages; i++) {
        if (pageTable[i].referenced < minTimestamp) {
            minTimestamp = pageTable[i].referenced;
            lruIndex = i;
        }
    }

    return lruIndex;
}

// Function to perform Working Set Clock (WSClock) page replacement
int wsclockReplacement(PageTableEntry pageTable[], int numPages, int *clockHand, int currentTime, int timeWindow) {
    int pageToReplace = -1;

    // Find pages belonging to the current process and check if any page is outside the working set
    for (int i = 0; i < numPages; i++) {
        if (pageTable[i].pageNumber != -1 && pageTable[i].inWorkingSet && (currentTime - pageTable[i].referenced) > timeWindow) {
            pageToReplace = i;
            break;
        }
    }

    // If no page is outside the working set, find the first page not in the working set
    if (pageToReplace == -1) {
        for (int i = 0; i < numPages; i++) {
            if (pageTable[i].pageNumber != -1 && !pageTable[i].inWorkingSet) {
                pageToReplace = i;
                break;
            }
        }
    }

    // If no page is found, use clock replacement to select a page to replace
    if (pageToReplace == -1) {
        while (true) {
            if (!pageTable[*clockHand].referenced) {
                pageToReplace = *clockHand;
                *clockHand = (*clockHand + 1) % numPages;
                break;
            } else {
                pageTable[*clockHand].referenced = false;
                *clockHand = (*clockHand + 1) % numPages;
            }
        }
    }

    return pageToReplace;
}

int main() {
    int numPages = 3; // Number of pages in memory
    PageTableEntry pageTable[numPages];
    int clockHand = 0;
    int timeWindow = 3; // Time window to define the working set

    // Initialize page table
    initializePageTable(pageTable, numPages);

    // Simulate page references
    int referenceString[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
    int numReferences = sizeof(referenceString) / sizeof(referenceString[0]);
    int currentTime = 0;

    printf("Reference String: ");
    for (int i = 0; i < numReferences; i++) {
        printf("%d ", referenceString[i]);
    }
    printf("\n");

    printf("\nSimulating Working Set Clock Page Replacement:\n");
    int pageFaults = 0;

    for (int i = 0; i < numReferences; i++) {
        int pageNumber = referenceString[i];

        // Check if page is already in memory and update its working set information
        bool pageFound = false;
        for (int j = 0; j < numPages; j++) {
            if (pageTable[j].pageNumber == pageNumber) {
                pageTable[j].referenced = currentTime;
                pageTable[j].inWorkingSet = true;
                pageFound = true;
                break;
            }
        }

        // If page is not in memory, perform page replacement
        if (!pageFound) {
            pageFaults++;
            int pageToReplace = wsclockReplacement(pageTable, numPages, &clockHand, currentTime, timeWindow);
            pageTable[pageToReplace].pageNumber = pageNumber;
            pageTable[pageToReplace].referenced = currentTime;
            pageTable[pageToReplace].inWorkingSet = true;
        }

        // Update current time
        currentTime++;

        // Print page table status after each memory access
        printf("Page Table Status after reference %d: ", pageNumber);
        for (int j = 0; j < numPages; j++) {
            if (pageTable[j].pageNumber != -1)
                printf("(%d, WS=%d) ", pageTable[j].pageNumber, pageTable[j].inWorkingSet ? 1 : 0);
            else
                printf("_ ");
        }
        printf("\n");
    }

    printf("Total page faults: %d\n", pageFaults);

    return 0;
}

