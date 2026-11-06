/* Baglam degistirme: calisan surecin CPU yazmaclari PCB'sine kaydedilir,
 * siradakinin PCB'sindeki degerler CPU'ya yuklenir. CPU yazmaclari global
 * degiskenlerle, surec tablosu iki PCB'lik bir diziyle taklit edilir. */
#include <stdio.h>

struct pcb { int pc, sp, r1; char durum; };
struct pcb P[2];   /* surec tablosu */
int pc, sp, r1;    /* CPU yazmaclari */

void baglam_degistir(int e, int y)
{
    P[e].pc = pc;  P[e].sp = sp;  /* kaydet */
    P[e].r1 = r1;  P[e].durum = 'H';
    pc = P[y].pc;  sp = P[y].sp;  /* yukle */
    r1 = P[y].r1;  P[y].durum = 'C';
}

static void yaz(const char *olay)
{
    printf("%-22s CPU: pc=%d sp=%d r1=%d | PCB0: %d %d %d %c | PCB1: %d %d %d %c\n",
           olay, pc, sp, r1,
           P[0].pc, P[0].sp, P[0].r1, P[0].durum,
           P[1].pc, P[1].sp, P[1].r1, P[1].durum);
}

int main(void)
{
    /* P0 calisiyor; P1'in son durumu PCB1'de sakli. */
    pc = 104; sp = 896; r1 = 8;
    P[0].durum = 'C';
    P[1].pc = 340; P[1].sp = 500; P[1].r1 = 2; P[1].durum = 'H';
    yaz("P0 calisiyor");

    /* kesme */  baglam_degistir(0, 1);
    yaz("baglam_degistir(0, 1)");

    /* ... P1 calisir ... */
    pc = 352; sp = 492; r1 = 5;
    yaz("P1 birkac komut");

    /* kesme */  baglam_degistir(1, 0);
    yaz("baglam_degistir(1, 0)");
    return 0;
}

/* Beklenen cikti:
 * P0 calisiyor           CPU: pc=104 sp=896 r1=8 | PCB0: 0 0 0 C | PCB1: 340 500 2 H
 * baglam_degistir(0, 1)  CPU: pc=340 sp=500 r1=2 | PCB0: 104 896 8 H | PCB1: 340 500 2 C
 * P1 birkac komut        CPU: pc=352 sp=492 r1=5 | PCB0: 104 896 8 H | PCB1: 340 500 2 C
 * baglam_degistir(1, 0)  CPU: pc=104 sp=896 r1=8 | PCB0: 104 896 8 C | PCB1: 352 492 5 H
 */
