#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Page table entry structure
typedef struct PageTableEntry {
    int pageNumber; // Page number
    bool referenced; // Reference bit
    bool modified; // Modified bit
} PageTableEntry;

// Function to initialize the page table
void initializePageTable(PageTableEntry pageTable[], int numPages) {
    for (int i = 0; i < numPages; i++) {
        pageTable[i].pageNumber = -1;
        pageTable[i].referenced = false;
        pageTable[i].modified = false;
    }
}

// Function to perform not recently used page replacement
int notRecentlyUsedReplacement(PageTableEntry pageTable[], int numPages) {
    int victimPage = -1;
    for (int i = 0; i < numPages; i++) {
        // Check for class 0: (referenced bit = 0, modified bit = 0)
        if (!pageTable[i].referenced && !pageTable[i].modified) {
            victimPage = i;
            break;
        }
    }

    // If no class 0 pages found, check for class 1: (referenced bit = 0, modified bit = 1)
    if (victimPage == -1) {
        for (int i = 0; i < numPages; i++) {
            if (!pageTable[i].referenced && pageTable[i].modified) {
                victimPage = i;
                break;
            }
        }
    }

    // If no class 0 or class 1 pages found, check for class 2: (referenced bit = 1, modified bit = 0)
    if (victimPage == -1) {
        for (int i = 0; i < numPages; i++) {
            if (pageTable[i].referenced && !pageTable[i].modified) {
                victimPage = i;
                break;
            }
        }
    }

    // If no class 0, class 1, or class 2 pages found, default to class 3: (referenced bit = 1, modified bit = 1)
    if (victimPage == -1) {
        for (int i = 0; i < numPages; i++) {
            if (pageTable[i].referenced && pageTable[i].modified) {
                victimPage = i;
                break;
            }
        }
    }

    // Clear the reference bit of the victim page
    pageTable[victimPage].referenced = false;

    return victimPage;
}

int main() {
    int numPages = 4; // Number of pages in memory
    PageTableEntry pageTable[numPages];

    // Initialize page table
    initializePageTable(pageTable, numPages);

    // Simulate page references
    int referenceString[] = {0, 1, 2, 3, 0, 1, 4, 0, 1, 2};
    int numReferences = sizeof(referenceString) / sizeof(referenceString[0]);
    printf("Reference String: ");
    for (int i = 0; i < numReferences; i++) {
        printf("%d ", referenceString[i]);
    }
    printf("\n");

    printf("\nSimulating Not Recently Used (NRU) Page Replacement:\n");
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
            int victimPage = notRecentlyUsedReplacement(pageTable, numPages);
            printf("Page %d is replaced by page %d\n", pageTable[victimPage].pageNumber, pageNumber);
            pageTable[victimPage].pageNumber = pageNumber;
            pageTable[victimPage].referenced = true; // Set reference bit
        }

        // Display current page table status
        printf("Page Table Status after reference %d: ", pageNumber);
        for (int j = 0; j < numPages; j++) {
            printf("(%d, R=%d, M=%d) ", pageTable[j].pageNumber, pageTable[j].referenced, pageTable[j].modified);
        }
        printf("\n");
    }

    return 0;
}

