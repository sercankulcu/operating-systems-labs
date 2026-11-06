/* Bellek eslemeli dosya: 12 KiB'lik (3 sayfa) dosya mmap ile eslenir; sayfalar
 * ilk erisimde hata ile okunur. p[5000] = 'x' sayfa 1'i kirletir, msync onu
 * dosyaya yazar. Dosya /tmp'de olusturulur ve sonda silinir. */
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define DOSYA "/tmp/veri.bin"

static int olustur(void)
{
    char blok[4096];
    int fd = open(DOSYA, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
        return -1;
    memset(blok, 'a', sizeof blok);
    for (int i = 0; i < 3; i++)
        if (write(fd, blok, sizeof blok) != (ssize_t)sizeof blok) {
            close(fd);
            return -1;
        }
    close(fd);
    return 0;
}

int main(void)
{
    if (olustur() != 0) { perror("olustur"); return 1; }

    int fd = open(DOSYA, O_RDWR);
    struct stat st;
    fstat(fd, &st);          /* 12 KiB = 3 sayfa */
    char *p = mmap(NULL, st.st_size,
         PROT_READ | PROT_WRITE,
         MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) { perror("mmap"); close(fd); unlink(DOSYA); return 1; }
    close(fd);               /* esleme surer */
    char c = p[0];           /* sayfa 0 */
    p[5000] = 'x';           /* sayfa 1 */
    msync(p, st.st_size, MS_SYNC);
    munmap(p, st.st_size);

    printf("boyut = %lld bayt (%lld sayfa)\n", (long long)st.st_size,
           (long long)st.st_size / 4096);
    printf("c = p[0] = '%c'\n", c);

    /* Degisiklik dosyada mi? read ile denetle */
    char b = 0;
    fd = open(DOSYA, O_RDONLY);
    if (pread(fd, &b, 1, 5000) == 1)
        printf("dosyada bayt 5000 = '%c' (sayfa %d)\n", b, 5000 / 4096);
    close(fd);
    unlink(DOSYA);
    return 0;
}

/* Beklenen cikti:
 * boyut = 12288 bayt (3 sayfa)
 * c = p[0] = 'a'
 * dosyada bayt 5000 = 'x' (sayfa 1)
 */
