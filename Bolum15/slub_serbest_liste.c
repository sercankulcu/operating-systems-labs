/* SLUB serbest listesi: 4 KiB'lik slab sayfasi 1 KiB'lik dort nesneye
 * bolunur; bos nesneler bir listede, "sonraki" isaretcisi bos nesnenin
 * kendi icinde saklanir. Ayirma listenin basini alir (LIFO); liste bosalinca
 * yavas yol yeni slab ayirir. kmem_cache kullanici alaninda benzetilir. */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define SAYFA 4096
#define NESNE 1024
#define MAXSAYFA 8

typedef struct {
    void *freelist;
    char *sayfa[MAXSAYFA];   /* benzetim: konum yazdirmak ve serbest birakmak icin */
    int nsayfa;
} kmem_cache;

/* yavas yol: sayfa ayiricidan yeni sayfa al, nesnelere bol, ilkini dondur */
static void *yeni_slab(kmem_cache *c)
{
    char *s = aligned_alloc(SAYFA, SAYFA);
    if (!s || c->nsayfa == MAXSAYFA) {
        fprintf(stderr, "bellek yok\n");
        exit(1);
    }
    c->sayfa[c->nsayfa++] = s;
    for (int i = 0; i < SAYFA / NESNE; i++)
        *(void **)(s + i * NESNE) = (i + 1 < SAYFA / NESNE) ? s + (i + 1) * NESNE : NULL;
    return s;
}

void *slab_ayir(kmem_cache *c)
{
    void *nesne = c->freelist;    /* listenin basi */
    if (!nesne)
        nesne = yeni_slab(c);     /* yavas yol */
    c->freelist = *(void **)nesne;  /* sonraki */
    return nesne;
}

void slab_birak(kmem_cache *c, void *nesne)
{
    *(void **)nesne = c->freelist;  /* nesnenin icine */
    c->freelist = nesne;
}

/* nesnenin konumu: ilk sayfa icin "+k", sonrakiler icin "n:+k" */
static const char *konum(kmem_cache *c, void *p)
{
    static char b[4][16];
    static int sira = 0;
    char *s = b[sira++ % 4];
    for (int i = 0; i < c->nsayfa; i++) {
        ptrdiff_t k = (char *)p - c->sayfa[i];
        if (k >= 0 && k < SAYFA) {
            if (i == 0)
                snprintf(s, 16, "+%td", k);
            else
                snprintf(s, 16, "%d:+%td", i + 1, k);
            return s;
        }
    }
    return "?";
}

int main(void)
{
    kmem_cache c = { NULL, { NULL }, 0 };
    c.freelist = yeni_slab(&c);       /* baslangic: bos bir slab */

    /* a,b = ayir; birak(a); c,d,e,f = ayir */
    void *a = slab_ayir(&c);
    void *b = slab_ayir(&c);
    printf("a = %s   b = %s", konum(&c, a), konum(&c, b));
    slab_birak(&c, a);
    void *cc = slab_ayir(&c);
    printf("   c = %s\n", konum(&c, cc));
    void *d = slab_ayir(&c);
    void *e = slab_ayir(&c);
    int once = c.nsayfa;
    void *f = slab_ayir(&c);
    printf("d = %s   e = %s   f = %s%s\n", konum(&c, d), konum(&c, e), konum(&c, f),
           c.nsayfa > once ? " (yeni slab)" : "");

    for (int i = 0; i < c.nsayfa; i++)
        free(c.sayfa[i]);
    return 0;
}

/* Beklenen cikti:
 * a = +0   b = +1024   c = +0
 * d = +2048   e = +3072   f = 2:+0 (yeni slab)
 */
