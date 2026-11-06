/* Once hata denetimi: parametreleri kilit ve bellek almadan once dogrulamak
 * hata yolundaki geri alma kodunu ortadan kaldirir. Cekirdek fonksiyonlari
 * (kmalloc, kfree, access_ok, lock/unlock) kullanici alaninda taklit edilir
 * ve kac kaynak alinip birakildigi sayilir. */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>

#define MAX 4096
#define GFP_KERNEL 0

static pthread_mutex_t tbl_lock = PTHREAD_MUTEX_INITIALIZER;
static int kilit_alma = 0, ayirma = 0;

static void lock(pthread_mutex_t *m)   { pthread_mutex_lock(m);   kilit_alma++; }
static void unlock(pthread_mutex_t *m) { pthread_mutex_unlock(m); }

static void *kmalloc(size_t n, int flags)
{
    (void)flags;
    ayirma++;
    return malloc(n);
}

static void kfree(void *p) { free(p); }

/* benzetim: kullanici adresi NULL degilse ve aralik tasmiyorsa gecerli */
static int access_ok(const void *u, size_t n)
{
    return u != NULL && UINTPTR_MAX - (uintptr_t)u >= n;
}

/* kotu: kaynak alindiktan sonra denetim */
static long kotu(const void *u, size_t n)
{
    void *buf;
    lock(&tbl_lock);
    buf = kmalloc(n, GFP_KERNEL);
    if (n > MAX || !access_ok(u, n)) {
        kfree(buf);
        unlock(&tbl_lock);
        return -EINVAL;
    }
    /* ... tabloyla is ... */
    kfree(buf);
    unlock(&tbl_lock);
    return 0;
}

/* iyi: once denetim, sonra kaynak */
static long iyi(const void *u, size_t n)
{
    void *buf;
    if (n > MAX || !access_ok(u, n))
        return -EINVAL; /* once denetle */
    buf = kmalloc(n, GFP_KERNEL);
    if (!buf)
        return -ENOMEM;
    lock(&tbl_lock);
    /* ... tabloyla is ... */
    unlock(&tbl_lock);
    kfree(buf);
    return 0;
}

static void dene(const char *ad, long (*f)(const void *, size_t),
                 const void *u, size_t n)
{
    kilit_alma = ayirma = 0;
    long r = f(u, n);
    printf("%-4s n = %5zu -> %3ld  (kilit %d, kmalloc %d)\n",
           ad, n, r, kilit_alma, ayirma);
}

int main(void)
{
    char kullanici[64] = { 0 };
    dene("kotu", kotu, kullanici, 64);
    dene("kotu", kotu, kullanici, 100000);   /* gecersiz: bosuna kaynak */
    dene("iyi", iyi, kullanici, 64);
    dene("iyi", iyi, kullanici, 100000);     /* gecersiz: kaynak alinmaz */
    return 0;
}

/* Beklenen cikti (EINVAL = 22):
 * kotu n =    64 ->   0  (kilit 1, kmalloc 1)
 * kotu n = 100000 -> -22  (kilit 1, kmalloc 1)
 * iyi  n =    64 ->   0  (kilit 1, kmalloc 1)
 * iyi  n = 100000 -> -22  (kilit 0, kmalloc 0)
 */
