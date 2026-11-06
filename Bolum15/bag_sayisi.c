/* Bag sayisi ve unlink: kati bag yeni bir dizin girdisidir ve i-node'un bag
 * sayisini artirir; sembolik bag yalniz bir yol tutar ve hedef silinince
 * sarkar. Bag sayisi 0 olsa da acik tanimlayici varken veri yasar.
 * Ornek gecici bir dizinde calisir; i-node numaralari makineye baglidir. */
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

static char dizin[] = "/tmp/bag_XXXXXX";

static void bag(const char *an, int fd)
{
    struct stat st;
    if (fstat(fd, &st) == 0)
        printf("  %-18s i-node bag sayisi = %lu\n", an, (unsigned long)st.st_nlink);
}

int main(void)
{
    if (!mkdtemp(dizin)) { perror("mkdtemp"); return 1; }
    if (chdir(dizin) != 0) { perror("chdir"); return 1; }

    int fd = open("a.txt", O_CREAT|O_RDWR, 0644);
    printf("fd = %d\n", fd);
    if (write(fd, "veri", 4) != 4)
        perror("write");
    bag("open:", fd);
    if (link("a.txt", "b.txt") != 0)      /* kati bag */
        perror("link");
    bag("link:", fd);
    if (symlink("a.txt", "s.txt") != 0)   /* sembolik */
        perror("symlink");
    bag("symlink:", fd);
    unlink("a.txt");
    bag("unlink(a.txt):", fd);
    int r1 = open("b.txt", O_RDONLY);
    printf("r1 = %d\n", r1);
    int r2 = open("s.txt", O_RDONLY);
    printf("r2 = %d (%s)\n", r2, errno == ENOENT ? "ENOENT" : strerror(errno));
    unlink("b.txt");
    bag("unlink(b.txt):", fd);

    char buf[8] = { 0 };
    ssize_t n = read(r1, buf, sizeof buf - 1);
    printf("  r1'den okunan: \"%s\" (%zd bayt; ad yok ama veri yasiyor)\n", buf, n);
    close(r1);
    close(fd);                   /* son tanimlayici: i-node ve veri serbest */

    unlink("s.txt");             /* temizlik: sarkan sembolik bag */
    if (chdir("/") != 0)
        perror("chdir");
    rmdir(dizin);
    return 0;
}

/* Beklenen cikti:
 * fd = 3
 *   open:              i-node bag sayisi = 1
 *   link:              i-node bag sayisi = 2
 *   symlink:           i-node bag sayisi = 2
 *   unlink(a.txt):     i-node bag sayisi = 1
 * r1 = 4
 * r2 = -1 (ENOENT)
 *   unlink(b.txt):     i-node bag sayisi = 0
 *   r1'den okunan: "veri" (4 bayt; ad yok ama veri yasiyor)
 * (fd numaralari, kalitimla acik baska tanimlayici yoksa 3 ve 4 olur)
 */
