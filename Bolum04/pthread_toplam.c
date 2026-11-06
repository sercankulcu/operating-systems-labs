/* Pthreads: olustur ve bekle. runner is parcacigi 1..5 toplamini paylasilan
 * toplam degiskenine yazar; main join ile bekleyip sonucu yazdirir.
 * (Silberschatz Sekil 4.11'den uyarlanmistir.) */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int toplam;           /* paylasilan */

void *runner(void *param)
{
    int ust = atoi(param);
    toplam = 0;
    for (int i = 1; i <= ust; i++)
        toplam += i;
    pthread_exit(0);
}

int main(void)
{
    pthread_t tid;
    pthread_create(&tid, NULL, runner, "5");
    pthread_join(tid, NULL);
    printf("toplam = %d\n", toplam);
    return 0;
}

/* Beklenen cikti:
 * toplam = 15
 */
