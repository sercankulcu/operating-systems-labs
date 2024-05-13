#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Page table entry structure
typedef struct PageTableEntry {
    int pageNumber; // Page number
    bool inWorkingSet; // Flag indicating whether the page is in the working set or not
} PageTableEntry;

// Function to initialize the page table
void initializePageTable(PageTableEntry pageTable[], int numPages) {
    for (int i = 0; i < numPages; i++) {
        pageTable[i].pageNumber = -1;
        pageTable[i].inWorkingSet = false;
    }
}

// Function to perform Working Set page replacement
int workingSetReplacement(PageTableEntry pageTable[], int numPages, int *currentProcess, int currentTime, int timeWindow) {
    int pageToReplace = -1;

    // Find pages belonging to the current process and check if any page is outside the working set
    for (int i = 0; i < numPages; i++) {
        if (pageTable[i].pageNumber != -1 && pageTable[i].inWorkingSet && (currentTime - pageTable[i].inWorkingSet) > timeWindow) {
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

    // If no page is found, use round-robin to select a page to replace
    if (pageToReplace == -1) {
        pageToReplace = *currentProcess;
        *currentProcess = (*currentProcess + 1) % numPages;
    }

    return pageToReplace;
}

int main() {
    int numPages = 4; // Number of pages in memory
    PageTableEntry pageTable[numPages];

    // Initialize page table
    initializePageTable(pageTable, numPages);

    // Simulate page references
    int referenceString[] = {0, 1, 2, 3, 0, 1, 4, 0, 1, 2};
    int numReferences = sizeof(referenceString) / sizeof(referenceString[0]);
    int currentTime = 0;
    int currentProcess = 0;
    int timeWindow = 3; // Time window to define the working set

    printf("Reference String: ");
    for (int i = 0; i < numReferences; i++) {
        printf("%d ", referenceString[i]);
    }
    printf("\n");

    printf("\nSimulating Working Set Page Replacement:\n");
    int pageFaults = 0;

    for (int i = 0; i < numReferences; i++) {
        int pageNumber = referenceString[i];

        // Check if page is already in memory and update its working set information
        bool pageFound = false;
        for (int j = 0; j < numPages; j++) {
            if (pageTable[j].pageNumber == pageNumber) {
                pageTable[j].inWorkingSet = true;
                pageFound = true;
                break;
            }
        }

        // If page is not in memory, perform page replacement
        if (!pageFound) {
            pageFaults++;
            int pageToReplace = workingSetReplacement(pageTable, numPages, &currentProcess, currentTime, timeWindow);
            pageTable[pageToReplace].pageNumber = pageNumber;
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

