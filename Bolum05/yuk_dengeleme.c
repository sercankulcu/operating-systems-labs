/* Yuk dengeleme (cekme / pull): kuyrugu bosalan CPU en yuklu CPU'yu bulur ve
 * aradaki farkin yarisi kadar isi onun kuyrugunun sonundan kendine tasir. */
#include <stdio.h>

#define NCPU 4
#define KAP 8

struct kuyruk { char is[KAP]; int n; };
struct kuyruk q[NCPU] = {
    {{'A', 'B', 'C'}, 3},
    {{0}, 0},
    {{'D', 'E', 'F', 'G', 'H'}, 5},
    {{'I', 'J'}, 2},
};

/* Kaynagin kuyrugunun sonundaki isi hedefin sonuna tasir. */
static void gocur(struct kuyruk *kaynak, struct kuyruk *hedef)
{
    char is = kaynak->is[--kaynak->n];
    hedef->is[hedef->n++] = is;
    printf("  %c gocer\n", is);
}

void dengele(int bu)     /* bostaki CPU */
{
    int en = bu;
    for (int c = 0; c < NCPU; c++)
        if (q[c].n > q[en].n) /* daha yuklu */
            en = c;
    int t = (q[en].n - q[bu].n) / 2;
    printf("dengele(%d): en yuklu CPU %d, tasinacak %d is\n", bu, en, t);
    for (int k = 0; k < t; k++)
        gocur(&q[en], &q[bu]); /* sondan */
}

static void yaz(void)
{
    for (int c = 0; c < NCPU; c++) {
        printf("  CPU %d (n=%d):", c, q[c].n);
        for (int i = 0; i < q[c].n; i++)
            printf(" %c", q[c].is[i]);
        printf("\n");
    }
}

int main(void)
{
    printf("once:\n");
    yaz();
    dengele(1);
    printf("sonra:\n");
    yaz();
    return 0;
}

/* Beklenen cikti:
 * once:
 *   CPU 0 (n=3): A B C
 *   CPU 1 (n=0):
 *   CPU 2 (n=5): D E F G H
 *   CPU 3 (n=2): I J
 * dengele(1): en yuklu CPU 2, tasinacak 2 is
 *   H gocer
 *   G gocer
 * sonra:
 *   CPU 0 (n=3): A B C
 *   CPU 1 (n=2): H G
 *   CPU 2 (n=3): D E F
 *   CPU 3 (n=2): I J
 */
