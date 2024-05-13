#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h> // For copy_to_user and copy_from_user functions

#define DEVICE_NAME "simple_char_device"
#define BUFFER_SIZE 1024

static char device_buffer[BUFFER_SIZE];
static int device_open_count = 0;

static int simple_char_device_open(struct inode *inode, struct file *file) {
    if (device_open_count) {
        return -EBUSY; // Device is already open
    }
    device_open_count++;
    return 0; // Success
}

static int simple_char_device_release(struct inode *inode, struct file *file) {
    device_open_count--;
    return 0; // Success
}

static ssize_t simple_char_device_read(struct file *file, char *buffer, size_t length, loff_t *offset) {
    int bytes_read = 0;
    while (length && (device_buffer[*offset] != '\0')) {
        put_user(device_buffer[*offset], buffer++);
        length--;
        bytes_read++;
        (*offset)++;
    }
    return bytes_read;
}

static ssize_t simple_char_device_write(struct file *file, const char *buffer, size_t length, loff_t *offset) {
    int bytes_written = 0;
    while (length && (*offset < BUFFER_SIZE)) {
        get_user(device_buffer[*offset], buffer++);
        length--;
        bytes_written++;
        (*offset)++;
    }
    return bytes_written;
}

static struct file_operations simple_char_device_fops = {
    .open = simple_char_device_open,
    .read = simple_char_device_read,
    .write = simple_char_device_write,
    .release = simple_char_device_release
};

static int __init simple_char_device_init(void) {
    int ret = register_chrdev(0, DEVICE_NAME, &simple_char_device_fops);
    if (ret < 0) {
        printk(KERN_ALERT "Failed to register a character device\n");
        return ret;
    }
    printk(KERN_INFO "Simple character device registered with major number %d\n", ret);
    return 0;
}

static void __exit simple_char_device_exit(void) {
    unregister_chrdev(MAJOR_NUMBER, DEVICE_NAME);
    printk(KERN_INFO "Simple character device unregistered\n");
}

module_init(simple_char_device_init);
module_exit(simple_char_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple character device driver for Linux user space");

