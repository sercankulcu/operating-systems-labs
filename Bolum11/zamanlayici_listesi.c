/* Zamanlayici listesi: bekleyen olaylar mutlak zamanlarla degil, bir
 * oncekine gore farklarla tutulur. ekle() araya girer ve yalniz bir farki
 * gunceller; tik() yalniz bastaki farki azaltir (tik basina O(1)). */
#include <stdio.h>

typedef struct Zam {
    const char *ad;
    int fark;                /* onceki zamanlayiciya gore tik farki */
    struct Zam *sonraki;
} Zam;

Zam *bas = NULL;
int simdi = 4200;            /* su anki zaman (tik) */

Zam *calistir(Zam *z)
{
    printf("%d: %s calisti\n", simdi, z->ad);
    return z->sonraki;       /* listeden cikar */
}

void ekle(Zam *z, int t)     /* t: tik */
{
    Zam **p = &bas;
    while (*p && (*p)->fark <= t) {
        t -= (*p)->fark;
        p = &(*p)->sonraki;
    }
    z->fark = t; z->sonraki = *p;
    if (*p) (*p)->fark -= t;
    *p = z;
}

void tik(void)
{
    simdi++;
    if (bas) bas->fark--;
    while (bas && bas->fark == 0)
        bas = calistir(bas);   /* cikar */
}

static void listele(void)
{
    printf("liste:");
    for (Zam *z = bas; z; z = z->sonraki)
        printf(" %s(%d)", z->ad, z->fark);
    printf("\n");
}

int main(void)
{
    static Zam A = { "A", 0, NULL }, B = { "B", 0, NULL }, C = { "C", 0, NULL },
               D = { "D", 0, NULL }, E = { "E", 0, NULL }, X = { "X", 0, NULL };

    /* olaylar 4203, 4207, 4213, 4215, 4216'da */
    ekle(&A, 3);
    ekle(&B, 7);
    ekle(&C, 13);
    ekle(&D, 15);
    ekle(&E, 16);
    listele();

    ekle(&X, 10);            /* 4210: B ile C arasina girer */
    listele();

    for (int k = 0; k < 16; k++)
        tik();
    listele();
    return 0;
}

/* Beklenen cikti:
 * liste: A(3) B(4) C(6) D(2) E(1)
 * liste: A(3) B(4) X(3) C(3) D(2) E(1)
 * 4203: A calisti
 * 4207: B calisti
 * 4210: X calisti
 * 4213: C calisti
 * 4215: D calisti
 * 4216: E calisti
 * liste:
 */
