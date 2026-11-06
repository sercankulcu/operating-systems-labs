/* Programlanmis G/C (PIO) ile yazici: CPU her karakter icin yazicinin
 * hazir olmasini mesgul bekleyerek yoklar ve karakteri veri yazmacina yazar.
 * Yazici ayri bir is parcacigi olarak benzetilir. */
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>

#define READY 0   /* veri yazmaci bos: yazici yeni karakter alabilir */

/* Benzetimde durum ve veri yazmaci ayni hucredir: veri yazmacina yazmak
 * yaziciyi hemen mesgul yapar (gercek donanimda oldugu gibi). */
static atomic_int yazmac = READY;
static atomic_int *printer_status_reg = &yazmac;
static atomic_int *printer_data_register = &yazmac;

static char buffer[64];      /* cekirdek tamponu */
static int yazdirilacak;     /* yazicinin basacagi karakter sayisi */
static pthread_t yazici;

/* --- yazici (aygit) --- */
static void *yazici_calis(void *arg)
{
    (void)arg;
    for (int k = 0; k < yazdirilacak; k++) {
        int c;
        while ((c = atomic_load(&yazmac)) == READY)
            ;                                   /* karakter bekle */
        struct timespec ts = { 0, 1000000 };    /* basmak 1 ms surer */
        nanosleep(&ts, NULL);
        putchar(c);
        fflush(stdout);
        atomic_store(&yazmac, READY);           /* yeniden hazir */
    }
    return NULL;
}

static void copy_from_user(char *hedef, const char *kaynak, int n)
{
    memcpy(hedef, kaynak, (size_t)n);
}

static void return_to_user(void)
{
    pthread_join(yazici, NULL);   /* benzetim: yazicinin bitmesini bekle */
    putchar('\n');
}

/* write sistem cagrisinin PIO govdesi */
static void sistem_cagrisi(const char *p, int count)
{
    int i;
    copy_from_user(buffer, p, count);
    yazdirilacak = count;
    pthread_create(&yazici, NULL, yazici_calis, NULL);
    for (i = 0; i < count; i++) {
        while (*printer_status_reg != READY)
            ;   /* mesgul bekleme */
        *printer_data_register = p[i];
    }
    return_to_user();
}

int main(void)
{
    sistem_cagrisi("ABCD", 4);
    return 0;
}

/* Beklenen cikti:
 * ABCD
 */
