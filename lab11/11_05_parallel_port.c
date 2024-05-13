#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define PARALLEL_PORT_DEVICE "/dev/lp0" // Adjust this if your parallel port device file is different

int main() {
    int parallel_port_fd;

    // Open the parallel port device file for writing
    parallel_port_fd = open(PARALLEL_PORT_DEVICE, O_WRONLY);
    if (parallel_port_fd == -1) {
        perror("Failed to open parallel port device");
        return 1;
    }

    // Data to be written to the parallel port
    unsigned char data = 0xFF; // Example data, adjust as needed

    // Write data to the parallel port
    if (write(parallel_port_fd, &data, sizeof(data)) == -1) {
        perror("Failed to write to parallel port");
        close(parallel_port_fd);
        return 1;
    }

    printf("Data written to the parallel port successfully.\n");

    // Close the parallel port device file
    close(parallel_port_fd);

    return 0;
}

