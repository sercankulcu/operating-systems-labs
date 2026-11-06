/* Kullanici duzeyinde thread_yield: is parcacigi kutuphanesi calisanin PC ve
 * SP'sini tablosuna kaydeder, siradaki HAZIR is parcacigini secip onun
 * degerlerini yukler. Cekirdege giris yoktur. CPU yazmaclari PC ve SP global
 * degiskenleriyle taklit edilir. */
#include <stdio.h>

enum { HAZIR, CALISIYOR, BEKLIYOR };
static const char *durum_adi[] = {"HAZIR", "CALISIYOR", "BEKLIYOR"};

struct tcb { int pc, sp, durum; } T[3];
int cur = 0;               /* calisan */
int PC, SP;                /* "CPU yazmaclari" */

void thread_yield(void)    /* kutuphanede */
{
    T[cur].pc = PC;  T[cur].sp = SP;
    T[cur].durum = HAZIR;
    do cur = (cur + 1) % 3;
    while (T[cur].durum != HAZIR);
    T[cur].durum = CALISIYOR;
    PC = T[cur].pc;  SP = T[cur].sp;
}

static void yaz(const char *olay)
{
    printf("%-18s cur=%d PC=0x%X SP=0x%X  [T0 %s, T1 %s, T2 %s]\n",
           olay, cur, (unsigned)PC, (unsigned)SP,
           durum_adi[T[0].durum], durum_adi[T[1].durum], durum_adi[T[2].durum]);
}

int main(void)
{
    /* T0 calisiyor, T1 bir kilidi bekliyor, T2 hazir. */
    T[0].durum = CALISIYOR;
    T[1].pc = 0x300; T[1].sp = 0xA000; T[1].durum = BEKLIYOR;
    T[2].pc = 0x480; T[2].sp = 0xB000; T[2].durum = HAZIR;
    cur = 0; PC = 0x124; SP = 0x8FF0;
    yaz("baslangic");

    thread_yield();            /* T0 gonullu birakir */
    yaz("T0 yield");

    PC = 0x48C; SP = 0xAFF8;   /* T2 biraz calisti */
    thread_yield();
    yaz("T2 yield");
    return 0;
}

/* Beklenen cikti:
 * baslangic          cur=0 PC=0x124 SP=0x8FF0  [T0 CALISIYOR, T1 BEKLIYOR, T2 HAZIR]
 * T0 yield           cur=2 PC=0x480 SP=0xB000  [T0 HAZIR, T1 BEKLIYOR, T2 CALISIYOR]
 * T2 yield           cur=0 PC=0x124 SP=0x8FF0  [T0 CALISIYOR, T1 BEKLIYOR, T2 HAZIR]
 */
