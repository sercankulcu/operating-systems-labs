/* Kilitsiz (lock-free) yigin: push kilit yerine tek bir atomik top isaretcisi ve
 * CAS dongusu kullanir. CAS basarisiz olursa C11 guncel top'u eski'ye yazar ve
 * yeniden denenir. Once slayttaki iz (T1 X'i eklerken T2 Y'yi ekler) oynatilir,
 * sonra 4 is parcacigi 10000'er dugum ekler. */
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdatomic.h>

typedef struct node {
    char ad;
    int deger;
    struct node *next;
} node_t;

static _Atomic(node_t *) top = NULL;

void push(node_t *n)
{
    node_t *eski = atomic_load(&top);
    do {
        n->next = eski;
    } while (!atomic_compare_exchange_weak(
                 &top, &eski, n));
}  /* basarisizsa eski = guncel top */

static void yigin_yaz(const char *baslik)
{
    printf("%-34s top", baslik);
    for (node_t *p = atomic_load(&top); p != NULL; p = p->next)
        printf(" -> %c", p->ad);
    printf("\n");
}

#define IS_PARCACIGI 4
#define ADET 10000
static node_t dugumler[IS_PARCACIGI][ADET];

static void *isci(void *arg)
{
    int t = *(int *)arg;
    for (int k = 0; k < ADET; k++) {
        dugumler[t][k].ad = '#';
        dugumler[t][k].deger = 1;
        push(&dugumler[t][k]);
    }
    return NULL;
}

int main(void)
{
    /* 1) Slayttaki iz */
    node_t A = { 'A', 0, NULL }, B = { 'B', 0, NULL };
    node_t X = { 'X', 0, NULL }, Y = { 'Y', 0, NULL };
    push(&B);
    push(&A);
    yigin_yaz("baslangic:");

    node_t *eski = atomic_load(&top);          /* T1: eski = A */
    X.next = eski;                              /* T1: X.next = A */
    push(&Y);                                   /* T2: CAS(top, A, Y) basarili */
    yigin_yaz("T2 Y'yi ekledi:");
    bool ok = atomic_compare_exchange_strong(&top, &eski, &X);
    printf("T1: CAS(top, A, X) %s, eski = %c\n", ok ? "basarili" : "basarisiz", eski->ad);
    X.next = eski;                              /* yeniden dene: X.next = Y */
    ok = atomic_compare_exchange_strong(&top, &eski, &X);
    printf("T1: CAS(top, Y, X) %s\n", ok ? "basarili" : "basarisiz");
    yigin_yaz("son:");

    /* 2) Gercek is parcaciklari */
    atomic_store(&top, NULL);
    pthread_t t[IS_PARCACIGI];
    int id[IS_PARCACIGI];
    for (int i = 0; i < IS_PARCACIGI; i++) {
        id[i] = i;
        pthread_create(&t[i], NULL, isci, &id[i]);
    }
    for (int i = 0; i < IS_PARCACIGI; i++)
        pthread_join(t[i], NULL);
    long n = 0;
    for (node_t *p = atomic_load(&top); p != NULL; p = p->next)
        n += p->deger;
    printf("%d is parcacigi x %d push: yigindaki dugum = %ld\n", IS_PARCACIGI, ADET, n);
    return 0;
}

/* Beklenen cikti (her calistirmada ayni):
 * baslangic:                         top -> A -> B
 * T2 Y'yi ekledi:                    top -> Y -> A -> B
 * T1: CAS(top, A, X) basarisiz, eski = Y
 * T1: CAS(top, Y, X) basarili
 * son:                               top -> X -> Y -> A -> B
 * 4 is parcacigi x 10000 push: yigindaki dugum = 40000
 */
