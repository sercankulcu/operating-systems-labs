/* Piyango (lottery) cizelgeleme: her dilimde bir bilet cekilir; listede
 * birikimli bilet sayisi cekilen sayiyi asan surec CPU'yu alir.
 * A 30, B 10, C 60 bilet. Ilk uc cekilis sunumdaki gibi sabittir (47, 35, 12),
 * ardindan srand(42) ile 10 000 cekilis yapilir. (OSTEP Bolum 9) */
#include <stdio.h>
#include <stdlib.h>

/* Linux'taki getrandom(2) sistem cagrisiyla karismasin diye ad degistirilir. */
#define getrandom ornek_getrandom

typedef struct node_t {
    const char *ad;
    int tickets;
    struct node_t *next;
} node_t;

static const int sabit_cekilis[] = {47, 35, 12};
static int cekilis_no = 0;

/* [alt, ust) araliginda bir sayi; ilk uc cekilis sabit. */
static int getrandom(int alt, int ust)
{
    if (cekilis_no < 3)
        return sabit_cekilis[cekilis_no++];
    return alt + rand() % (ust - alt);
}

static int calisma_sayisi[3];
static int ayrintili = 1;

static void calistir(node_t *n)
{
    if (ayrintili)
        printf("%s calisir\n", n->ad);
    calisma_sayisi[n->ad[0] - 'A']++;
}

static node_t *head;
static int totaltickets = 0;

static void dilim(void)
{
    /* her dilimde bir cekilis */
    int counter = 0;
    int winner = getrandom(0, totaltickets);
    node_t *current = head;
    while (current) {
        counter = counter + current->tickets;
        if (counter > winner)
            break;          /* kazanan bulundu */
        current = current->next;
    }
    /* current: CPU'yu alacak surec */
    if (ayrintili)
        printf("winner=%d -> ", winner);
    calistir(current);
}

int main(void)
{
    static node_t C = {"C", 60, NULL};
    static node_t B = {"B", 10, &C};
    static node_t A = {"A", 30, &B};
    head = &A;
    for (node_t *n = head; n; n = n->next)
        totaltickets += n->tickets;

    for (int i = 0; i < 3; i++)
        dilim();

    srand(42);
    ayrintili = 0;
    for (int i = 0; i < 3; i++)
        calisma_sayisi[i] = 0;
    for (int i = 0; i < 10000; i++)
        dilim();
    printf("10000 dilim: A %d, B %d, C %d\n",
           calisma_sayisi[0], calisma_sayisi[1], calisma_sayisi[2]);
    return 0;
}

/* Beklenen cikti (son satirdaki sayilar glibc rand()'a baglidir; yaklasik
 * 3000 / 1000 / 6000 olur):
 * winner=47 -> C calisir
 * winner=35 -> B calisir
 * winner=12 -> A calisir
 * 10000 dilim: A ~3000, B ~1000, C ~6000
 */
