/* Dairesel tampon: uretici koy() ile kuy (yazma) indisinden yazar, tuketici
 * al() ile bas (okuma) indisinden okur. N yuvali tampon en cok N - 1 bayt
 * tutar; bir yuva "dolu" ile "bos"u ayirt etmek icin bos birakilir. */
#include <stdio.h>

#define N 5

char buf[N];
int bas = 0, kuy = 0;

int koy(char c)            /* uretici */
{
    int s = (kuy + 1) % N;
    if (s == bas) return -1; /* dolu */
    buf[kuy] = c;
    kuy = s;
    return 0;
}

int al(char *c)            /* tuketici */
{
    if (bas == kuy) return -1; /* bos */
    *c = buf[bas];
    bas = (bas + 1) % N;
    return 0;
}

static void koy_yaz(char c)
{
    if (koy(c) < 0)
        printf("koy('%c'): dolu\n", c);
}

static void al_yaz(void)
{
    char c;
    if (al(&c) == 0)
        printf("al: %c\n", c);
    else
        printf("al: bos\n");
}

int main(void)
{
    koy_yaz('a');
    koy_yaz('b');
    koy_yaz('c');
    al_yaz();
    koy_yaz('d');
    koy_yaz('e');      /* kuy basa sarar: kuy = 0 */
    koy_yaz('f');      /* (0 + 1) % 5 == bas: dolu */
    al_yaz();
    printf("bas = %d, kuy = %d\n", bas, kuy);
    return 0;
}

/* Beklenen cikti:
 * al: a
 * koy('f'): dolu
 * al: b
 * bas = 2, kuy = 0
 */
