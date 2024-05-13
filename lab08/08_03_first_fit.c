#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE 100

// Structure to represent a memory block
typedef struct MemoryBlock {
    int size;             // Size of the block
    int isFree;           // Flag to indicate whether the block is free (1) or allocated (0)
    struct MemoryBlock *next;  // Pointer to the next block
} MemoryBlock;

MemoryBlock *memory = NULL;

// Function to initialize memory with a single free block
void initializeMemory() {
    memory = (MemoryBlock *)malloc(sizeof(MemoryBlock));
    memory->size = MEMORY_SIZE;
    memory->isFree = 1;
    memory->next = NULL;
}

// Function to allocate memory using first fit strategy
void *firstFitAllocate(int size) {
    MemoryBlock *current = memory;
    while (current != NULL) {
        if (current->isFree && current->size >= size) {
            // Allocate memory
            if (current->size > size) {
                // Split the block if it's larger than requested
                MemoryBlock *newBlock = (MemoryBlock *)malloc(sizeof(MemoryBlock));
                newBlock->size = current->size - size;
                newBlock->isFree = 1;
                newBlock->next = current->next;
                current->next = newBlock;
                current->size = size;
            }
            current->isFree = 0;
            return (void *)(current + 1); // Return pointer to the data area of the block
        }
        current = current->next;
    }
    return NULL; // Memory allocation failed
}

// Function to free allocated memory
void deallocate(void *ptr) {
    if (ptr == NULL) return;
    MemoryBlock *block = (MemoryBlock *)ptr - 1;
    block->isFree = 1;
}

// Function to display memory blocks
void displayMemory() {
    MemoryBlock *current = memory;
    printf("Memory Layout:\n");
    while (current != NULL) {
        printf("Size: %d, Free: %s\n", current->size, current->isFree ? "Yes" : "No");
        current = current->next;
    }
}

int main() {
    initializeMemory();
    
    displayMemory();
    // Allocate memory using first fit
    void *ptr1 = firstFitAllocate(20);
    displayMemory();
    void *ptr2 = firstFitAllocate(30);

    // Display memory layout after allocation
    displayMemory();

    // Free memory
    deallocate(ptr1);
    
    // Display memory layout after deallocation
    displayMemory();

    // Free memory
    deallocate(ptr2);
    
    // Display memory layout after deallocation
    displayMemory();

    // Free memory allocated for memory block
    free(memory);

    return 0;
}

/* We simulate a simple memory management system with a linked list of memory blocks.
firstFitAllocate() searches for the first free block that is large enough to hold the requested size using the first fit strategy. If found, it allocates the memory and returns a pointer to the data area of the block. If not found, it returns NULL.
*/
