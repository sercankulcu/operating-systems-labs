// Include necessary headers
#include <stdio.h>     // For input/output
#include <stdlib.h>    // For malloc and exit
#include <stdbool.h>   // For boolean type
#include <stdint.h>

// Define constants
#define PAGE_SIZE 4096         // Page size in bytes (4KB)
#define NUM_PAGES 8            // Number of pages in virtual memory
#define PHYSICAL_MEMORY_SIZE (PAGE_SIZE * 4)  // 16KB physical memory
#define VIRTUAL_ADDRESS_SPACE (PAGE_SIZE * NUM_PAGES)  // 32KB virtual address space

// Structure for page table entry
struct PageTableEntry {
    int frame_number;    // Physical frame number (-1 if not mapped)
    bool present;       // Page present in physical memory
};

// Global variables
struct PageTableEntry page_table[NUM_PAGES];  // Page table for virtual-to-physical mapping
int physical_memory[PHYSICAL_MEMORY_SIZE / sizeof(int)];  // Simulated physical memory (in integers)

// Function prototypes
void initialize_mmu();
unsigned int translate_address(unsigned int virtual_address, bool *fault);
void load_page(int page_number, int frame_number);
void print_page_table();
void print_memory_state(int virtual_address, unsigned int physical_address, bool fault);

int main() {
    // Initialize MMU and page table
    initialize_mmu();
    
    // Print initial page table state
    print_page_table();
    
    // Simulate some memory accesses
    unsigned int virtual_addresses[] = {0x0000, 0x1000, 0x2000, 0x5000, 0x8000};
    int num_accesses = sizeof(virtual_addresses) / sizeof(virtual_addresses[0]);
    
    printf("\nSimulating memory accesses:\n");
    for (int i = 0; i < num_accesses; i++) {
        bool page_fault = false;
        unsigned int physical_address = translate_address(virtual_addresses[i], &page_fault);
        print_memory_state(virtual_addresses[i], physical_address, page_fault);
        
        // Simulate writing to memory if page is present
        if (!page_fault) {
            int index = physical_address / sizeof(int);
            physical_memory[index] = i + 100;  // Write some data
            printf("Wrote value %d to physical address 0x%04x\n", 
                   physical_memory[index], physical_address);
        }
    }
    
    return 0;
}

// Initialize MMU and page table
void initialize_mmu() {
    // Initially, only first 4 pages are mapped to physical frames
    for (int i = 0; i < NUM_PAGES; i++) {
        if (i < 4) {  // Map first 4 pages
            page_table[i].frame_number = i;  // Frame 0-3
            page_table[i].present = true;
        } else {  // Remaining pages not in memory
            page_table[i].frame_number = -1;
            page_table[i].present = false;
        }
    }

    // Initialize physical memory with zeros (fixed warning with size_t)
    size_t memory_size = PHYSICAL_MEMORY_SIZE / sizeof(uint32_t);
    for (size_t i = 0; i < memory_size; i++) {
        physical_memory[i] = 0;
    }
}

// Translate virtual address to physical address
unsigned int translate_address(unsigned int virtual_address, bool *fault) {
    // Extract page number and offset
    unsigned int page_number = virtual_address / PAGE_SIZE;
    unsigned int offset = virtual_address % PAGE_SIZE;
    
    // Check if page number is valid
    if (page_number >= NUM_PAGES) {
        printf("Error: Virtual address 0x%04x exceeds address space\n", virtual_address);
        *fault = true;
        return 0;
    }
    
    // Check if page is present
    if (!page_table[page_number].present) {
        printf("Page fault: Page %d not in physical memory\n", page_number);
        // Simulate page loading (in real OS, this would involve disk I/O)
        load_page(page_number, page_number % 4);  // Simple frame allocation
        *fault = true;
    }
    
    // Calculate physical address
    unsigned int frame_number = page_table[page_number].frame_number;
    unsigned int physical_address = (frame_number * PAGE_SIZE) + offset;
    
    *fault = false;  // No fault if we reach here without loading
    return physical_address;
}

// Load a page into physical memory (simple simulation)
void load_page(int page_number, int frame_number) {
    printf("Loading page %d into frame %d\n", page_number, frame_number);
    page_table[page_number].frame_number = frame_number;
    page_table[page_number].present = true;
    
    // In a real system, this would copy data from disk to physical memory
    // Here, we just mark it as present
}

// Print page table contents
void print_page_table() {
    printf("Page Table:\n");
    printf("Page | Frame | Present\n");
    for (int i = 0; i < NUM_PAGES; i++) {
        printf("%4d | %5d | %s\n", i, 
               page_table[i].frame_number, 
               page_table[i].present ? "Yes" : "No");
    }
}

// Print memory access result
void print_memory_state(int virtual_address, unsigned int physical_address, bool fault) {
    printf("\nVirtual Address: 0x%04x\n", virtual_address);
    if (!fault) {
        printf("Physical Address: 0x%04x\n", physical_address);
    }
}
