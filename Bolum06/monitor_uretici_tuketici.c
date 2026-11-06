/* Monitorle uretici-tuketici (N = 2). C'de monitor yok: monitorun ortulu kilidi
 * bir mutex, full ve empty kosullari pthread kosul degiskenleridir. Her
 * procedure girisinde kilit alinir, cikista birakilir. Hoare anlaminda "if"
 * yeterlidir; pthread Mesa anlamli oldugu icin burada "while" kullanilir.
 * Tuketici once gelir (empty'de bekler), uretici a, b, c, d koyar. */
#include <stdio.h>
#include <pthread.h>
#include <time.h>

/* <stdio.h> remove() ile karismasin */
#define remove pc_remove

#define N 2

/* monitor ProducerConsumer */
static pthread_mutex_t monitor_kilidi = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t full = PTHREAD_COND_INITIALIZER;
static pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
static int count = 0;
static int tampon[N], bas = 0, son = 0;
static int en_cok = 0;            /* gorulen en buyuk count */
static int empty_bekleyen = 0;

static void insert_item(int item) { tampon[son] = item; son = (son + 1) % N; }
static int remove_item(void) { int x = tampon[bas]; bas = (bas + 1) % N; return x; }

void insert(int item)
{
    pthread_mutex_lock(&monitor_kilidi);
    while (count == N) pthread_cond_wait(&full, &monitor_kilidi);   /* if count = N then wait(full) */
    insert_item(item);
    count = count + 1;
    if (count > en_cok) en_cok = count;
    if (count == 1) pthread_cond_signal(&empty);
    pthread_mutex_unlock(&monitor_kilidi);
}

int remove(void)
{
    pthread_mutex_lock(&monitor_kilidi);
    while (count == 0) {                                             /* if count = 0 then wait(empty) */
        empty_bekleyen++;
        pthread_cond_wait(&empty, &monitor_kilidi);
        empty_bekleyen--;
    }
    int r = remove_item();
    count = count - 1;
    if (count == N - 1) pthread_cond_signal(&full);
    pthread_mutex_unlock(&monitor_kilidi);
    return r;
}
/* end monitor */

#define ADET 4
static int alinan[ADET];

static void *tuketici(void *arg)
{
    (void)arg;
    for (int k = 0; k < ADET; k++)
        alinan[k] = remove();
    return NULL;
}

static void *uretici(void *arg)
{
    (void)arg;
    for (int k = 0; k < ADET; k++)
        insert('a' + k);
    return NULL;
}

int main(void)
{
    pthread_t t, u;
    pthread_create(&t, NULL, tuketici, NULL);
    for (;;) {                               /* T empty'de bekleyene dek */
        pthread_mutex_lock(&monitor_kilidi);
        int b = empty_bekleyen;
        pthread_mutex_unlock(&monitor_kilidi);
        if (b == 1)
            break;
        struct timespec ts = { 0, 1000000 };
        nanosleep(&ts, NULL);
    }
    printf("T remove(): tampon bos, empty'de bekliyor\n");
    pthread_create(&u, NULL, uretici, NULL);
    pthread_join(u, NULL);
    pthread_join(t, NULL);

    printf("T aldi:");
    for (int k = 0; k < ADET; k++)
        printf(" %c", alinan[k]);
    printf("\ncount = %d, en buyuk count = %d (N = %d asilmaz)\n",
           count, en_cok <= N ? en_cok : -1, N);
    return 0;
}

/* Beklenen cikti (ilk iki satir her calistirmada ayni; "en buyuk count"
 * zamanlamaya gore 1 ya da 2 olabilir):
 * T remove(): tampon bos, empty'de bekliyor
 * T aldi: a b c d
 * count = 0, en buyuk count = 2 (N = 2 asilmaz)
 */
