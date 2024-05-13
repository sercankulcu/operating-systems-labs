#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

// Define S_IFSOCK macro if it's not provided by the system
#ifndef S_IFSOCK
#define S_IFSOCK 0140000
#endif

// Define S_IFMT macro if it's not provided by the system
#ifndef S_IFMT
#define S_IFMT 0170000
#endif

// Define S_ISSOCK macro if it's not provided by the system
#ifndef S_ISSOCK
#define S_ISSOCK(mode) (((mode) & S_IFMT) == S_IFSOCK)
#endif

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct stat fileStat;
    if (stat(argv[1], &fileStat) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    // File type
    printf("File: %s\n", argv[1]);
    printf("File type: ");

    if (S_ISBLK(fileStat.st_mode)) {
        printf("block device\n");
    } else if (S_ISCHR(fileStat.st_mode)) {
        printf("character device\n");
    } else if (S_ISDIR(fileStat.st_mode)) {
        printf("directory\n");
    } else if (S_ISFIFO(fileStat.st_mode)) {
        printf("FIFO/pipe\n");
    } else if (S_ISLNK(fileStat.st_mode)) {
        printf("symlink\n");
    } else if (S_ISREG(fileStat.st_mode)) {
        printf("regular file\n");
    } else if (S_ISSOCK(fileStat.st_mode)) {
        printf("socket\n");
    } else {
        printf("unknown?\n");
    }

    // File permissions
    printf("File permissions: ");
    printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf((fileStat.st_mode & S_IXOTH) ? "x\n" : "-\n");

    // File size
    printf("File size: %ld bytes\n", fileStat.st_size);

    // Owner user ID and group ID
    printf("Owner UID: %d\n", fileStat.st_uid);
    printf("Group GID: %d\n", fileStat.st_gid);

    // Last access time
    printf("Last access time: %s", ctime(&fileStat.st_atime));

    // Last modification time
    printf("Last modification time: %s", ctime(&fileStat.st_mtime));

    // Last status change time
    printf("Last status change time: %s", ctime(&fileStat.st_ctime));

    return 0;
}

