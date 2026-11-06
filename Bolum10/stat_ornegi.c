/* stat yapisi ve ls -l: bir dosyanin i-node bilgileri stat() ile okunur ve
 * ls -l biciminde yazilir. struct stat <sys/stat.h>'ta tanimlidir:
 *   dev_t st_dev; ino_t st_ino; mode_t st_mode; nlink_t st_nlink; uid_t st_uid;
 *   gid_t st_gid; dev_t st_rdev; off_t st_size; blksize_t st_blksize;
 *   blkcnt_t st_blocks; struct timespec st_atim, st_mtim, st_ctim; */
#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define DOSYA "/tmp/stat_ornek.txt"

static char tur_harfi(mode_t m)
{
    if (S_ISREG(m)) return '-';
    if (S_ISDIR(m)) return 'd';
    if (S_ISLNK(m)) return 'l';
    if (S_ISCHR(m)) return 'c';
    if (S_ISBLK(m)) return 'b';
    if (S_ISFIFO(m)) return 'p';
    if (S_ISSOCK(m)) return 's';
    return '?';
}

static void ls_l(const char *yol)
{
    struct stat st;
    char izin[11];
    char zaman[32];
    const char *rwx = "rwxrwxrwx";

    if (stat(yol, &st) != 0) {
        perror(yol);
        return;
    }
    izin[0] = tur_harfi(st.st_mode);
    for (int i = 0; i < 9; i++)
        izin[i + 1] = (st.st_mode & (0400 >> i)) ? rwx[i] : '-';
    if (st.st_mode & S_ISVTX)               /* yapiskan bit: /tmp */
        izin[9] = (st.st_mode & S_IXOTH) ? 't' : 'T';
    izin[10] = '\0';
    strftime(zaman, sizeof zaman, "%Y-%m-%d %H:%M", localtime(&st.st_mtim.tv_sec));

    printf("%s %lu %u %u %lld %s %s\n", izin, (unsigned long)st.st_nlink,
           (unsigned)st.st_uid, (unsigned)st.st_gid, (long long)st.st_size,
           zaman, yol);
    printf("  st_ino = %lu, st_blksize = %ld, st_blocks = %lld (512 B birim)\n",
           (unsigned long)st.st_ino, (long)st.st_blksize, (long long)st.st_blocks);
}

int main(void)
{
    const char metin[] = "isletim sistemleri\n";
    int fd = open(DOSYA, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd < 0) { perror("open"); return 1; }
    if (write(fd, metin, strlen(metin)) < 0)
        perror("write");
    fchmod(fd, 0644);                       /* umask'tan bagimsiz */
    close(fd);

    ls_l(DOSYA);
    ls_l("/tmp");
    unlink(DOSYA);
    return 0;
}

/* Beklenen cikti (i-node, uid/gid, zaman ve blok sayilari sisteme gore degisir):
 * -rw-r--r-- 1 1001 1001 19 2026-11-06 12:00 /tmp/stat_ornek.txt
 *   st_ino = 1234567, st_blksize = 4096, st_blocks = 8 (512 B birim)
 * drwxrwxrwt 12 0 0 4096 2026-11-06 12:00 /tmp
 *   st_ino = 2, st_blksize = 4096, st_blocks = 8 (512 B birim)
 */
