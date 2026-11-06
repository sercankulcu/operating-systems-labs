/* Canli kilit (livelock): A once resource_1, B once resource_2 ister (Tanenbaum 6-16).
 * 1) enter_region/leave_region C11 atomic_flag ile mesgul beklemeli kilittir.
 * 2) "Kibar" surecler ayni ritimde al-dene-birak yapinca hicbiri ilerlemez;
 *    rastgele geri cekilme (srand(42)) dongu kirar. Adimli, tek is parcacikli benzetim. */
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* ---- 1) Gercek kod: mesgul beklemeli kilitler ---- */
static atomic_flag resource_1 = ATOMIC_FLAG_INIT;
static atomic_flag resource_2 = ATOMIC_FLAG_INIT;
static int kullanim = 0;

static void enter_region(atomic_flag *r)
{
    while (atomic_flag_test_and_set(r)) {
        /* mesgul bekle: bloke olmaz, CPU harcar */
    }
}

static void leave_region(atomic_flag *r)
{
    atomic_flag_clear(r);
}

static void use_both_resources(void)
{
    kullanim++;
}

static void process_A(void)
{
    enter_region(&resource_1);
    enter_region(&resource_2);
    use_both_resources();
    leave_region(&resource_2);
    leave_region(&resource_1);
}

static void process_B(void)            /* B: once resource_2, sonra 1 */
{
    enter_region(&resource_2);
    enter_region(&resource_1);
    use_both_resources();
    leave_region(&resource_1);
    leave_region(&resource_2);
}

static void *calis_A(void *arg)
{
    (void)arg;
    process_A();
    return NULL;
}

static void *calis_B(void *arg)
{
    (void)arg;
    process_B();
    return NULL;
}

/* ---- 2) Adimli benzetim: kibar surecler ---- */
#define MAX_TUR 30

struct surec {
    char ad;
    int ilk, ikinci;            /* kaynak numarasi: 0 = r1, 1 = r2 */
    int faz;                    /* 0: ilki al, 1: ikinciyi dene, 2: bitti */
    int bekle;                  /* kalan geri cekilme turu */
    int bitis;
};

static int benzet(bool rastgele, bool yaz)
{
    struct surec s[2] = { { 'A', 0, 1, 0, 0, 0 }, { 'B', 1, 0, 0, 0, 0 } };
    int sahip[2] = { -1, -1 };

    for (int t = 1; t <= MAX_TUR; t++) {
        int once[2] = { sahip[0], sahip[1] };   /* tur basindaki durum */
        int birak[2] = { -1, -1 };              /* tur sonunda birakilacak */

        if (yaz)
            printf("tur %2d:", t);
        for (int i = 0; i < 2; i++) {
            struct surec *p = &s[i];
            if (p->faz == 2)
                continue;
            if (p->bekle > 0) {
                p->bekle--;
                if (yaz)
                    printf("  %c bekliyor", p->ad);
                continue;
            }
            if (p->faz == 0) {
                if (once[p->ilk] == -1 && sahip[p->ilk] == -1) {
                    sahip[p->ilk] = i;
                    p->faz = 1;
                    if (yaz)
                        printf("  %c r%d aldi", p->ad, p->ilk + 1);
                }
            } else if (once[p->ikinci] == -1 && sahip[p->ikinci] == -1) {
                sahip[p->ikinci] = i;             /* ikisi de elde: calis */
                birak[i] = 1;
                p->faz = 2;
                p->bitis = t;
                if (yaz)
                    printf("  %c r%d aldi, calisti", p->ad, p->ikinci + 1);
            } else {
                birak[i] = 0;                     /* kibarca birak */
                p->faz = 0;
                p->bekle = rastgele ? rand() % 4 : 0;
                if (yaz)
                    printf("  %c r%d alamadi, r%d birakti", p->ad,
                           p->ikinci + 1, p->ilk + 1);
            }
        }
        if (yaz)
            printf("\n");
        for (int i = 0; i < 2; i++) {
            if (birak[i] >= 0)
                sahip[s[i].ilk] = -1;
            if (birak[i] == 1)
                sahip[s[i].ikinci] = -1;
        }
        if (s[0].faz == 2 && s[1].faz == 2) {
            printf("A %d. turda, B %d. turda bitti\n", s[0].bitis, s[1].bitis);
            return t;
        }
        if (!rastgele && t == 6) {
            printf("6 turda kimse ilerlemedi: canli kilit (sonsuza dek surer)\n");
            return -1;
        }
    }
    printf("%d turda bitmedi\n", MAX_TUR);
    return -1;
}

int main(void)
{
    pthread_t t;

    /* A ve B ard arda calisir: ayni anda calissalar sonsuza dek donebilirlerdi */
    pthread_create(&t, NULL, calis_A, NULL);
    pthread_join(t, NULL);
    pthread_create(&t, NULL, calis_B, NULL);
    pthread_join(t, NULL);
    printf("ard arda: use_both_resources %d kez calisti\n\n", kullanim);

    printf("Kibar surecler, ayni ritim:\n");
    benzet(false, true);

    printf("\nRastgele geri cekilme (0-3 tur):\n");
    srand(42);
    benzet(true, true);
    return 0;
}

/* Beklenen cikti (2. bolumun ayrintisi glibc rand() dizisine baglidir):
 * ard arda: use_both_resources 2 kez calisti
 *
 * Kibar surecler, ayni ritim:
 * tur  1:  A r1 aldi  B r2 aldi
 * tur  2:  A r2 alamadi, r1 birakti  B r1 alamadi, r2 birakti
 * tur  3:  A r1 aldi  B r2 aldi
 * tur  4:  A r2 alamadi, r1 birakti  B r1 alamadi, r2 birakti
 * tur  5:  A r1 aldi  B r2 aldi
 * tur  6:  A r2 alamadi, r1 birakti  B r1 alamadi, r2 birakti
 * 6 turda kimse ilerlemedi: canli kilit (sonsuza dek surer)
 *
 * Rastgele geri cekilme (0-3 tur):
 * tur  1:  A r1 aldi  B r2 aldi
 * tur  2:  A r2 alamadi, r1 birakti  B r1 alamadi, r2 birakti
 * tur  3:  A bekliyor  B r2 aldi
 * tur  4:  A bekliyor  B r1 aldi, calisti
 * tur  5:  A r1 aldi
 * tur  6:  A r2 aldi, calisti
 * A 6. turda, B 4. turda bitti
 */
