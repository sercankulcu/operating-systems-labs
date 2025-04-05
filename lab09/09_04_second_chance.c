#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Page table entry structure
typedef struct PageTableEntry {
    int pageNumber; // Page number
    bool referenced; // Reference bit
} PageTableEntry;

// Function to initialize the page table
void initializePageTable(PageTableEntry pageTable[], int numPages) {
    for (int i = 0; i < numPages; i++) {
        pageTable[i].pageNumber = -1;
        pageTable[i].referenced = false;
    }
}

// Function to perform Second Chance page replacement
int secondChanceReplacement(PageTableEntry pageTable[], int numPages, int *currentIndex) {
    while (true) {
        if (pageTable[*currentIndex].referenced) {
            pageTable[*currentIndex].referenced = false;
            *currentIndex = (*currentIndex + 1) % numPages;
        } else {
            int victimIndex = *currentIndex;
            *currentIndex = (*currentIndex + 1) % numPages; // Advance after replacement
            return victimIndex;
        }
    }
}

int main() {
    int numPages = 3; // Number of pages in memory
    PageTableEntry pageTable[numPages];

    // Initialize page table
    initializePageTable(pageTable, numPages);

    // Simulate page references
    int referenceString[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
    int numReferences = sizeof(referenceString) / sizeof(referenceString[0]);
    printf("Reference String: ");
    for (int i = 0; i < numReferences; i++) {
        printf("%d ", referenceString[i]);
    }
    printf("\n");

    printf("\nSimulating Second Chance Page Replacement:\n");
    int currentIndex = 0;
    int pageFaults = 0;

    for (int i = 0; i < numReferences; i++) {
        int pageNumber = referenceString[i];

        // Check if page is already in memory
        bool pageFault = true;
        for (int j = 0; j < numPages; j++) {
            if (pageTable[j].pageNumber == pageNumber) {
                pageTable[j].referenced = true; // Set reference bit
                pageFault = false;
                break;
            }
        }

        // If page is not in memory, perform page replacement
        if (pageFault) {
            pageFaults++;
            int indexToReplace = secondChanceReplacement(pageTable, numPages, &currentIndex);
            printf("Page %d replaced by %d\n", pageTable[indexToReplace].pageNumber, pageNumber);
            pageTable[indexToReplace].pageNumber = pageNumber;
            pageTable[indexToReplace].referenced = true;
        }

        // Display current page table status
        printf("Page Table Status after reference %d: ", pageNumber);
        for (int j = 0; j < numPages; j++) {
            printf("(%d, R=%d) ", pageTable[j].pageNumber, pageTable[j].referenced);
        }
        printf("\n");
    }

    printf("Total page faults: %d\n", pageFaults);

    return 0;
}
