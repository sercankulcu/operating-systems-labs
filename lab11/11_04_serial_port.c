#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#define SERIAL_PORT "/dev/ttyS0"   // Serial port device name
#define BAUDRATE B9600             // Baud rate

int main() {
    int fd;
    struct termios serial_settings;

    // Open serial port
    fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        perror("Failed to open serial port");
        return -1;
    }

    // Configure serial port settings
    memset(&serial_settings, 0, sizeof(serial_settings));
    serial_settings.c_cflag = BAUDRATE | CS8 | CREAD | CLOCAL;
    serial_settings.c_iflag = IGNPAR;
    serial_settings.c_oflag = 0;
    serial_settings.c_lflag = 0;
    serial_settings.c_cc[VTIME] = 0;    // Timeout in tenths of a second
    serial_settings.c_cc[VMIN] = 1;     // Minimum number of characters to read
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &serial_settings);

    // Send data
    char tx_buffer[] = "Hello, Serial Port!\n";
    if (write(fd, tx_buffer, strlen(tx_buffer)) < 0) {
        perror("Error writing to serial port");
        close(fd);
        return -1;
    }

    // Receive data
    char rx_buffer[256];
    ssize_t bytes_read;
    while (1) {
        bytes_read = read(fd, rx_buffer, sizeof(rx_buffer));
        if (bytes_read > 0) {
            rx_buffer[bytes_read] = '\0';
            printf("Received: %s", rx_buffer);
        } else if (bytes_read < 0) {
            perror("Error reading from serial port");
            close(fd);
            return -1;
        }
    }

    // Close serial port
    close(fd);

    return 0;
}

