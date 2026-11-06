/* Yigin kanaryasi: derleyici buf ile kayitli rbp arasina ilk bayti 0x00
 * olan rastgele bir deger koyar ve donmeden once denetler. Cerceve bir bayt
 * dizisiyle benzetilir: [0..7] buf, [8..15] kanarya, [16..23] rbp,
 * [24..31] donus adresi. Tasan cagri SIGABRT'yi bir cocuk surecte alir. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>

/* Gercek __stack_chk_guard/__stack_chk_fail glibc'ye aittir; benzetimde
 * ayni adlar kendi degiskenimize ve fonksiyonumuza yonlendirilir. */
#define __stack_chk_guard sim_stack_chk_guard
#define __stack_chk_fail  sim_stack_chk_fail

static long __stack_chk_guard;              /* surec basinda rastgele */
static unsigned char cerceve[32];

static void __stack_chk_fail(void)
{
    printf("*** stack smashing detected ***: terminated\n");
    fflush(stdout);
    abort();                                 /* SIGABRT */
}

/* kanarya satirlarini derleyici ekler */
void kopyala(const char *girdi)
{
    long kanarya = __stack_chk_guard;
    char *buf = (char *)cerceve;             /* char buf[8]; */
    memcpy(cerceve + 8, &kanarya, 8);        /* giris kodu: kanaryayi koy */

    strcpy(buf, girdi);

    memcpy(&kanarya, cerceve + 8, 8);        /* cikis kodu: yigindan oku */
    printf("kopyala(\"%s\"): kanarya = 0x%016lx\n", girdi, (unsigned long)kanarya);
    fflush(stdout);
    if (kanarya != __stack_chk_guard)
        __stack_chk_fail();
}

int main(void)
{
    srand(42);
    __stack_chk_guard = ((long)rand() << 32 | (long)rand()) & ~0xFFL;   /* ilk bayt 0x00 */

    kopyala("AAAA");                          /* sigar: kanarya saglam */
    printf("donuldu\n");
    fflush(stdout);

    pid_t pid = fork();
    if (pid == 0) {
        struct rlimit yok = { 0, 0 };
        setrlimit(RLIMIT_CORE, &yok);         /* cekirdek dokumu yazilmasin */
        /* girdi: "AAAAAAAABBBBBBBB" */
        kopyala("AAAAAAAABBBBBBBB");
        printf("donuldu\n");                  /* buraya gelinmez */
        _exit(0);
    }
    int st;
    waitpid(pid, &st, 0);
    if (WIFSIGNALED(st))
        printf("cocuk sinyal %d (%s) ile sonlandi\n", WTERMSIG(st),
               WTERMSIG(st) == SIGABRT ? "SIGABRT" : "?");
    return 0;
}

/* Beklenen cikti (ilk kanarya degeri srand(42)'ye ve libc'nin rand'ina
 * baglidir; son bayti her zaman 00):
 * kopyala("AAAA"): kanarya = 0x........xxxxxx00
 * donuldu
 * kopyala("AAAAAAAABBBBBBBB"): kanarya = 0x4242424242424242
 * *** stack smashing detected ***: terminated
 * cocuk sinyal 6 (SIGABRT) ile sonlandi
 */
