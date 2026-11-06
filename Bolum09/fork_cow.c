/* fork() ve yazinca kopyalama (COW): fork yalniz sayfa tablolarini kopyalar.
 * Cocuk x'i okuyunca kopya olmaz; yazinca cekirdek D sayfasini kopyalar.
 * Ebeveynin x'i 1 kalir. Cocugun yazmada aldigi kucuk hata sayisi da yazilir.
 * Bir programin hata sayilarini disaridan gormek icin (sunumdaki komutlar):
 *   $ /usr/bin/time -v ./prog | grep -i fault
 *   $ ps -o min_flt,maj_flt -p PID
 *   $ perf stat -e page-faults,major-faults ./prog */
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int x = 1;                 /* veri: D */

static long kucuk_hata(void)
{
    struct rusage r;
    getrusage(RUSAGE_SELF, &r);
    return r.ru_minflt;
}

int main(void)
{
    char *b = malloc(4096);  /* heap: H */
    b[0] = 'a';
    pid_t pid = fork();
    if (pid == 0) {          /* cocuk */
        printf("%d\n", x);    /* okuma */
        long once = kucuk_hata();
        x = 2;                 /* yazma */
        long sonra = kucuk_hata();
        printf("cocuk: x = %d, yazmada kucuk hata: %ld (COW kopyasi)\n", x, sonra - once);
        fflush(stdout);        /* _exit stdio tamponunu bosaltmaz */
        _exit(0);
    }
    wait(NULL);
    b[0] = 'b';              /* ebeveyn */
    printf("ebeveyn: x = %d, b[0] = %c\n", x, b[0]);
    free(b);
    return 0;
}

/* Beklenen cikti (hata sayisi cekirdege gore degisebilir, genelde 1):
 * 1
 * cocuk: x = 2, yazmada kucuk hata: 1 (COW kopyasi)
 * ebeveyn: x = 1, b[0] = b
 */
