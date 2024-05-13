#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>

#define GPIO_PIN_NUMBER 17 // Example GPIO pin number (change it according to your setup)

int main() {
    // Open the GPIO export file to enable the GPIO pin
    int export_fd = open("/sys/class/gpio/export", O_WRONLY);
    if (export_fd < 0) {
        perror("Failed to open GPIO export file");
        return 1;
    }

    // Export the GPIO pin
    dprintf(export_fd, "%d", GPIO_PIN_NUMBER);
    close(export_fd);

    // Open the GPIO direction file to set the pin as an input
    char gpio_direction_path[128];
    snprintf(gpio_direction_path, sizeof(gpio_direction_path), "/sys/class/gpio/gpio%d/direction", GPIO_PIN_NUMBER);
    int direction_fd = open(gpio_direction_path, O_WRONLY);
    if (direction_fd < 0) {
        perror("Failed to open GPIO direction file");
        return 1;
    }

    // Set the GPIO pin as an input
    dprintf(direction_fd, "in");
    close(direction_fd);

    // Open the GPIO value file for reading
    char gpio_value_path[128];
    snprintf(gpio_value_path, sizeof(gpio_value_path), "/sys/class/gpio/gpio%d/value", GPIO_PIN_NUMBER);
    int value_fd = open(gpio_value_path, O_RDONLY);
    if (value_fd < 0) {
        perror("Failed to open GPIO value file");
        return 1;
    }

    printf("Listening for GPIO interrupts. Press Ctrl+C to exit.\n");

    // Set up a file descriptor set for select() to monitor the GPIO pin
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(value_fd, &fds);

    // Use select() to wait for changes in the GPIO pin state
    while (1) {
        if (select(value_fd + 1, &fds, NULL, NULL, NULL) < 0) {
            perror("select");
            break;
        }

        // Read the GPIO pin value to clear the interrupt
        char value;
        if (read(value_fd, &value, 1) != 1) {
            perror("read");
            break;
        }

        // Print a message when an interrupt occurs (GPIO pin state changes)
        printf("Interrupt detected on GPIO pin %d\n", GPIO_PIN_NUMBER);
    }

    // Clean up: Close file descriptors and unexport GPIO pin
    close(value_fd);

    int unexport_fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (unexport_fd < 0) {
        perror("Failed to open GPIO unexport file");
        return 1;
    }

    dprintf(unexport_fd, "%d", GPIO_PIN_NUMBER);
    close(unexport_fd);

    return 0;
}

