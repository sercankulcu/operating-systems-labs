/* Bekleyen sinyalleri secme: cekirdek kullanici moduna donerken
 * hazir = bekleyen & ~engelli kumesinden en kucuk numarali sinyali secer ve
 * teslim eder. Engellenen sinyal kaybolmaz, bekler. Gorev yapisi ve teslim
 * kullanici alaninda benzetilir (SIGINT sureci gercekten sonlandirmaz). */
#include <stdio.h>
#include <signal.h>

typedef struct {
    unsigned long bekl;      /* bekleyen sinyaller: bit (sig - 1) */
    unsigned long engl;      /* engellenen sinyaller */
} task;

#define BIT(sig) (1UL << ((sig) - 1))

static void teslim_et(task *t, int sig)
{
    (void)t;
    if (sig == SIGUSR1)
        printf("teslim: %d (SIGUSR1)\n", sig);
    else if (sig == SIGCHLD)
        printf("teslim: %d (SIGCHLD)\n", sig);
    else if (sig == SIGINT)
        printf("teslim: %d (SIGINT -> varsayilan: sonlandir)\n", sig);
    else
        printf("teslim: %d\n", sig);
}

/* kullanici moduna donerken (cekirdek) */
void sinyal_bak(task *t)
{
    unsigned long h = t->bekl & ~t->engl;
    while (h) {
        int sig = __builtin_ctzl(h) + 1;
        t->bekl &= ~(1UL << (sig - 1));
        teslim_et(t, sig); /* isleyici/varsayilan */
        h = t->bekl & ~t->engl;
    }
}

int main(void)
{
    task t = { 0, 0 };

    /* once:  sigprocmask(SIG_BLOCK, {SIGINT}) */
    t.engl |= BIT(SIGINT);
    /* surec uyurken: Ctrl-C (2), kill -USR1 (10), cocuk bitti (17) */
    t.bekl |= BIT(SIGINT) | BIT(SIGUSR1) | BIT(SIGCHLD);
    printf("kullanici moduna donus:\n");
    sinyal_bak(&t);
    printf("hala bekleyen: %s\n", (t.bekl & BIT(SIGINT)) ? "SIGINT" : "-");

    /* sonra: sigprocmask(SIG_UNBLOCK, {SIGINT}) */
    t.engl &= ~BIT(SIGINT);
    printf("sigprocmask(SIG_UNBLOCK) donusu:\n");
    sinyal_bak(&t);
    return 0;
}

/* Beklenen cikti (Linux x86-64: SIGINT 2, SIGUSR1 10, SIGCHLD 17):
 * kullanici moduna donus:
 * teslim: 10 (SIGUSR1)
 * teslim: 17 (SIGCHLD)
 * hala bekleyen: SIGINT
 * sigprocmask(SIG_UNBLOCK) donusu:
 * teslim: 2 (SIGINT -> varsayilan: sonlandir)
 */
