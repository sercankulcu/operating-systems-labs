/* Olceklenebilir (yaklasik) sayac: her cekirdegin kendi yerel sayaci ve kilidi var;
 * yerel deger S esigine ulasinca genel sayaca aktarilir. oku() hizlidir ama
 * yaklasiktir: en fazla NCPU * (S - 1) geride kalir. Once slayttaki tablo
 * (NCPU = 4, S = 4) oynatilir, sonra 4 is parcacigi gercekten sayar. */
#include <stdio.h>
#include <pthread.h>

#define NCPU 4

typedef struct {
    int global;      pthread_mutex_t gkilit;
    int yerel[NCPU]; pthread_mutex_t ykilit[NCPU];
    int S;                     /* aktarma esigi */
} ycount_t;

static void ycount_baslat(ycount_t *c, int S)
{
    c->global = 0;
    pthread_mutex_init(&c->gkilit, NULL);
    for (int i = 0; i < NCPU; i++) {
        c->yerel[i] = 0;
        pthread_mutex_init(&c->ykilit[i], NULL);
    }
    c->S = S;
}

static void ycount_yok_et(ycount_t *c)
{
    pthread_mutex_destroy(&c->gkilit);
    for (int i = 0; i < NCPU; i++)
        pthread_mutex_destroy(&c->ykilit[i]);
}

void ekle(ycount_t *c, int cpu)
{
    pthread_mutex_lock(&c->ykilit[cpu]);
    c->yerel[cpu]++;
    if (c->yerel[cpu] >= c->S) {        /* aktar */
        pthread_mutex_lock(&c->gkilit);
        c->global += c->yerel[cpu];
        pthread_mutex_unlock(&c->gkilit);
        c->yerel[cpu] = 0;
    }
    pthread_mutex_unlock(&c->ykilit[cpu]);
}

int oku(ycount_t *c)                    /* yaklasik */
{
    pthread_mutex_lock(&c->gkilit);
    int v = c->global;
    pthread_mutex_unlock(&c->gkilit);
    return v;
}

static int gercek;

static void satir(ycount_t *c, int t)
{
    printf("%d   %2d %2d %2d %2d   %2d   %2d\n", t, c->yerel[0], c->yerel[1],
           c->yerel[2], c->yerel[3], c->global, gercek);
}

/* bir zaman adiminda verilen cekirdekler ekle() cagirir (1 tabanli, 0 = son) */
static void adim(ycount_t *c, int t, const int *cekirdekler)
{
    for (const int *p = cekirdekler; *p != 0; p++) {
        ekle(c, *p - 1);
        gercek++;
    }
    satir(c, t);
}

static ycount_t paylasilan;
#define TUR 100000

static void *isci(void *arg)
{
    int cpu = *(int *)arg;
    for (int k = 0; k < TUR; k++)
        ekle(&paylasilan, cpu);
    return NULL;
}

int main(void)
{
    ycount_t c;
    ycount_baslat(&c, 4);
    printf("t   Y1 Y2 Y3 Y4    G  gercek\n");
    satir(&c, 0);
    static const int t1[] = { 1, 3, 0 };       /* cekirdek 1 ve 3 */
    static const int t2[] = { 1, 2, 3, 0 };
    static const int t3[] = { 1, 3, 4, 0 };
    static const int t4[] = { 1, 2, 0 };       /* Y1 esige ulasir: G = 4 */
    static const int t5[] = { 3, 4, 0 };       /* Y3 esige ulasir: G = 8 */
    adim(&c, 1, t1);
    adim(&c, 2, t2);
    adim(&c, 3, t3);
    adim(&c, 4, t4);
    adim(&c, 5, t5);
    printf("oku() = %d, gercek = %d, en kotu fark NCPU*(S-1) = %d\n",
           oku(&c), gercek, NCPU * (c.S - 1));
    ycount_yok_et(&c);

    /* 4 is parcacigi, S = 1024 */
    pthread_t t[NCPU];
    int id[NCPU];
    ycount_baslat(&paylasilan, 1024);
    for (int i = 0; i < NCPU; i++) {
        id[i] = i;
        pthread_create(&t[i], NULL, isci, &id[i]);
    }
    for (int i = 0; i < NCPU; i++)
        pthread_join(t[i], NULL);
    int kalan = 0;
    for (int i = 0; i < NCPU; i++)
        kalan += paylasilan.yerel[i];
    printf("S = 1024: oku() = %d, yerellerde kalan = %d, toplam = %d\n",
           oku(&paylasilan), kalan, oku(&paylasilan) + kalan);
    ycount_yok_et(&paylasilan);
    return 0;
}

/* Beklenen cikti (her calistirmada ayni):
 * t   Y1 Y2 Y3 Y4    G  gercek
 * 0    0  0  0  0    0    0
 * 1    1  0  1  0    0    2
 * 2    2  1  2  0    0    5
 * 3    3  1  3  1    0    8
 * 4    0  2  3  1    4   10
 * 5    0  2  0  2    8   12
 * oku() = 8, gercek = 12, en kotu fark NCPU*(S-1) = 12
 * S = 1024: oku() = 397312, yerellerde kalan = 2688, toplam = 400000
 * (her cekirdek 97 kez 1024 aktarir: 99328; 672 yerelde kalir)
 */
