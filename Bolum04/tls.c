/* Is parcacigina yerel depolama (TLS): duz global tum is parcaciklarinca
 * paylasilir; _Thread_local (C11) her is parcacigina ayri kopya verir. */
#include <stdio.h>
#include <pthread.h>

int ortak_hata;                 /* duz global: tek kopya */
_Thread_local int hata;         /* her is parcacigina ayri kopya */

struct sonuc { int hata, ortak_hata; };

static void *calis(void *arg)
{
    struct sonuc *r = arg;
    hata = r->hata;             /* kendi kopyasina yazar */
    ortak_hata = r->hata;       /* herkesin gordugune yazar */
    r->hata = hata;             /* kendi kopyasini geri okur */
    return NULL;
}

int main(void)
{
    struct sonuc A = {1, 0}, B = {2, 0};
    pthread_t ta, tb;
    pthread_create(&ta, NULL, calis, &A);
    pthread_join(ta, NULL);
    pthread_create(&tb, NULL, calis, &B);
    pthread_join(tb, NULL);

    printf("A: hata=%d\n", A.hata);
    printf("B: hata=%d\n", B.hata);
    printf("main: hata=%d (kendi kopyasi, baslangic degeri)\n", hata);
    printf("main: ortak_hata=%d (son yazan B)\n", ortak_hata);
    return 0;
}

/* Beklenen cikti:
 * A: hata=1
 * B: hata=2
 * main: hata=0 (kendi kopyasi, baslangic degeri)
 * main: ortak_hata=2 (son yazan B)
 */
