/* Arguman gecirme ve sonuc alma: birden cok deger bir yapiya konup adresi
 * gecirilir; sonuc heap'te ayrilip pthread_join ile main'e teslim edilir. */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct { int alt, ust; } aralik_t;
typedef struct { long toplam; int adet; } sonuc_t;

void *topla(void *arg)
{
    aralik_t *a = arg;          /* void* -> tur */
    sonuc_t *s = malloc(sizeof *s); /* heap'te */
    if (s == NULL)
        return NULL;
    s->toplam = 0; s->adet = 0;
    for (int i = a->alt; i <= a->ust; i++) {
        s->toplam += i; s->adet++;
    }
    return s;         /* = pthread_exit(s) */
}

int main(void)
{
    aralik_t a = { 1, 100 };
    pthread_t t;  void *ret;
    pthread_create(&t, NULL, topla, &a);
    pthread_join(t, &ret);          /* ret = s */
    sonuc_t *s = ret;
    if (s == NULL)
        return 1;
    printf("%d sayi, toplam %ld\n", s->adet, s->toplam);
    free(s);
    return 0;
}

/* Beklenen cikti:
 * 100 sayi, toplam 5050
 */
