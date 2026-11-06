/* CAS ile atomik artirma: oku, sonra "degismediyse yaz"; degistiyse yeniden dene.
 * Once slayttaki iz (v = 5, T1 ve T2 ayni anda 5 okur, T2 kazanir, T1 yeniden
 * dener, v = 7) adim adim oynatilir; sonra iki is parcacigi artir() cagirir. */
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

#define TUR 100000

int compare_and_swap(atomic_int *v, int bekl, int yeni)
{
    int eski = bekl;
    (void)atomic_compare_exchange_strong(v, &eski, yeni);
    return eski;
}

void artir(atomic_int *v)
{
    int temp;
    do {
        temp = atomic_load(v);              /* oku              */
    } while (temp != compare_and_swap(v, temp, temp + 1));
}                                           /* degismediyse yaz */

static atomic_int sayac = 0;
static atomic_int sayac2 = 0;

static void *isci(void *arg)
{
    (void)arg;
    for (int k = 0; k < TUR; k++) {
        artir(&sayac);
        atomic_fetch_add(&sayac2, 1);       /* C11 tek satir */
    }
    return NULL;
}

int main(void)
{
    /* Slayttaki iz, elle araya sokulmus */
    atomic_int v = 5;
    int t1_temp = atomic_load(&v);          /* T1 okur: 5 */
    int t2_temp = atomic_load(&v);          /* T2 okur: 5 */
    printf("v = %d, T1.temp = %d, T2.temp = %d\n", atomic_load(&v), t1_temp, t2_temp);
    int r = compare_and_swap(&v, t2_temp, t2_temp + 1);
    printf("T2: CAS(v, %d, %d) eski = %d -> basarili, v = %d\n",
           t2_temp, t2_temp + 1, r, atomic_load(&v));
    r = compare_and_swap(&v, t1_temp, t1_temp + 1);
    printf("T1: CAS(v, %d, %d) eski = %d -> basarisiz, yeniden dene\n",
           t1_temp, t1_temp + 1, r);
    t1_temp = atomic_load(&v);              /* T1 yeniden okur: 6 */
    r = compare_and_swap(&v, t1_temp, t1_temp + 1);
    printf("T1: CAS(v, %d, %d) eski = %d -> basarili, v = %d\n",
           t1_temp, t1_temp + 1, r, atomic_load(&v));

    pthread_t t[2];
    for (int i = 0; i < 2; i++)
        pthread_create(&t[i], NULL, isci, NULL);
    for (int i = 0; i < 2; i++)
        pthread_join(t[i], NULL);
    printf("artir()          : %d\n", atomic_load(&sayac));
    printf("atomic_fetch_add : %d\n", atomic_load(&sayac2));
    return 0;
}

/* Beklenen cikti (her calistirmada ayni):
 * v = 5, T1.temp = 5, T2.temp = 5
 * T2: CAS(v, 5, 6) eski = 5 -> basarili, v = 6
 * T1: CAS(v, 5, 6) eski = 6 -> basarisiz, yeniden dene
 * T1: CAS(v, 6, 7) eski = 6 -> basarili, v = 7
 * artir()          : 200000
 * atomic_fetch_add : 200000
 */
