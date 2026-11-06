/* Sistem cagrisi dagitimi: cekirdegin tek giris noktasi, cagri numarasini
 * fonksiyon isaretcisi tablosunda arar. Cekirdek kullanici alaninda taklit
 * edilir; numaralar Linux x86-64 ile ayni (0 read, 1 write, 2 open). */
#include <stdio.h>
#include <errno.h>

/* "Cekirdek" fonksiyonlari: gercek is yapmaz, ne istendigini yazar. */
static long sys_read(long fd, long buf, long n)
{
    (void)buf;
    printf("  [cekirdek] sys_read(fd=%ld, n=%ld)\n", fd, n);
    return 0;
}

static long sys_write(long fd, long buf, long n)
{
    printf("  [cekirdek] sys_write(fd=%ld, \"%s\", %ld)\n", fd, (const char *)buf, n);
    return n;                       /* yazilan bayt sayisi */
}

static long sys_open(long yol, long bayrak, long kip)
{
    (void)kip;
    printf("  [cekirdek] sys_open(\"%s\", %ld)\n", (const char *)yol, bayrak);
    return 3;                       /* ilk bos tanimlayici */
}

typedef long (*cagri_t)(long, long, long);
cagri_t tablo[3] = {sys_read, sys_write,
                    sys_open};

long syscall_giris(long no, long a,
                   long b, long c)
{
    if (no < 0 || no >= 3)
        return -ENOSYS;     /* gecersiz numara */
    return tablo[no](a, b, c);
}

/* libc sarmalayicisi: negatif donusu errno'ya cevirip -1 dondurur. */
static long libc_syscall(long no, long a, long b, long c)
{
    long r = syscall_giris(no, a, b, c);
    if (r < 0) {
        errno = (int)-r;
        return -1;
    }
    return r;
}

int main(void)
{
    long r;

    printf("write(1, \"abc\", 3), no=1\n");
    r = libc_syscall(1, 1, (long)"abc", 3);
    printf("  donus=%ld\n", r);

    printf("open(\"notlar.txt\", 0), no=2\n");
    r = libc_syscall(2, (long)"notlar.txt", 0, 0);
    printf("  donus=%ld\n", r);

    printf("syscall(99)\n");
    r = syscall_giris(99, 0, 0, 0);
    printf("  cekirdek donusu=%ld\n", r);
    r = libc_syscall(99, 0, 0, 0);
    printf("  donus=%ld, errno=%d (ENOSYS=%d)\n", r, errno, ENOSYS);
    return 0;
}

/* Beklenen cikti:
 * write(1, "abc", 3), no=1
 *   [cekirdek] sys_write(fd=1, "abc", 3)
 *   donus=3
 * open("notlar.txt", 0), no=2
 *   [cekirdek] sys_open("notlar.txt", 0)
 *   donus=3
 * syscall(99)
 *   cekirdek donusu=-38
 *   donus=-1, errno=38 (ENOSYS=38)
 */
