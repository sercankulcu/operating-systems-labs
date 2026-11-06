/* Birden cok is parcacigi (veri paralelligi): 1..1000 toplami dort is
 * parcacigina bolunur; her biri yalnizca kendi kismi[k] hucresine yazar. */
#include <pthread.h>
#include <stdio.h>

#define N 4
long kismi[N];           /* her biri kendi hucresine */

void *topla(void *arg)
{
    long k = (long)arg;    /* 0 .. N-1 */
    for (long i = k*250 + 1; i <= (k+1)*250; i++)
        kismi[k] += i;
    return NULL;
}

int main(void)
{
    pthread_t t[N];
    for (long k = 0; k < N; k++)
        pthread_create(&t[k], NULL, topla, (void *)k);
    long toplam = 0;
    for (int k = 0; k < N; k++) {
        pthread_join(t[k], NULL);
        toplam += kismi[k];
    }
    printf("%ld\n", toplam);  /* 500500 */
    return 0;
}

/* Beklenen cikti:
 * 500500
 */
