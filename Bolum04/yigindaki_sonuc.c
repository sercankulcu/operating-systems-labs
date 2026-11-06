/* Yaygin hata: is parcacigi sonucunu kendi yigininda tutup adresini
 * dondurmemelidir; fonksiyon donunce o cerceve yok olur. Dogru surum sonucu
 * heap'te ayirir. Hatali surum derlenmez: gcc -Wreturn-local-addr uyarir. */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct { long toplam; int adet; } sonuc_t;

#if 0   /* HATALI SURUM: derlenirse tanimsiz davranis */
void *topla(void *arg) {
  sonuc_t s = { 0, 0 };  /* T'nin yigininda */
  /* ... s doldurulur ... */
  return &s;            /* HATA */
}
#endif

/* Dogru surum: 1..ust toplanir. */
void *topla(void *arg)
{
    int ust = *(int *)arg;
    sonuc_t *s = malloc(sizeof *s);
    if (s == NULL)
        return NULL;
    /* ... s doldurulur ... */
    s->toplam = 0; s->adet = 0;
    for (int i = 1; i <= ust; i++) {
        s->toplam += i; s->adet++;
    }
    return s;    /* heap'te: yasar */
}

int main(void)
{
    int ust = 10;
    pthread_t t;
    void *ret;
    pthread_create(&t, NULL, topla, &ust);
    pthread_join(t, &ret);
    sonuc_t *s = ret;
    if (s == NULL)
        return 1;
    printf("%d sayi, toplam %ld\n", s->adet, s->toplam);
    free(s);
    return 0;
}

/* Beklenen cikti:
 * 10 sayi, toplam 55
 */
