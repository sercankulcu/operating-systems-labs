/* Yoklama (polling) ile el sikisma: surucu BUSY bitini mesgul beklemeyle
 * yoklar, komut ve veri yazmaclarini doldurup CMDRDY'yi acar. Denetleyici
 * ayri bir is parcacigi olarak benzetilir. */
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

/* durum yazmaci bitleri */
#define BUSY   0x1
#define ERR    0x2
/* komut yazmaci bitleri */
#define WRITE  0x1
#define CMDRDY 0x80

/* denetleyicinin yazmaclari (gercekte aygit adresleri) */
static atomic_int status = BUSY;      /* basta onceki is suruyor */
static atomic_int command = 0;
static atomic_int data_out = 0;

static char aygit[8];                 /* aygita giden baytlar */
static int aygit_n = 0;
static long bekleme_turu = 0;         /* surucunun bos donus sayisi */

/* --- denetleyici tarafi --- */
static void *denetleyici(void *arg)
{
    (void)arg;
    /* onceki is bitiyor: BUSY = 0 */
    for (volatile int k = 0; k < 100000; k++)
        ;
    atomic_store(&status, 0);

    while (!(atomic_load(&command) & CMDRDY))
        ;                                         /* CMDRDY'yi bekle */
    atomic_fetch_or(&status, BUSY);               /* CMDRDY goruldu -> BUSY = 1 */
    if (atomic_load(&command) & WRITE)            /* komutu oku */
        aygit[aygit_n++] = (char)atomic_load(&data_out);  /* bayti aygita yaz */
    atomic_store(&command, 0);                    /* CMDRDY = 0 */
    atomic_fetch_and(&status, ~(ERR | BUSY));     /* ERR = BUSY = 0 */
    return NULL;
}

int main(void)
{
    pthread_t d;
    pthread_create(&d, NULL, denetleyici, NULL);

    /* --- surucu (ana makine) tarafi --- */
    while (atomic_load(&status) & BUSY)
        bekleme_turu++;          /* mesgul bekleme */
    atomic_store(&command, WRITE);
    atomic_store(&data_out, 'A');
    atomic_fetch_or(&command, CMDRDY);

    pthread_join(d, NULL);
    printf("aygita yazilan: %.*s\n", aygit_n, aygit);
    printf("status = %d (ERR = 0: basarili)\n", atomic_load(&status));
    printf("surucu %s bekledi\n", bekleme_turu > 0 ? "mesgul" : "hic");
    return 0;
}

/* Beklenen cikti:
 * aygita yazilan: A
 * status = 0 (ERR = 0: basarili)
 * surucu mesgul bekledi
 * (bekleme turu sayisi zamanlamaya baglidir; is parcacigi gec baslarsa
 *  son satir "hic bekledi" de olabilir) */
