/* Yol onbellegi ile ad cozumleme: once onbellekteki en uzun onek aranir,
 * kalan bilesenler diskten (i-node + dizin blogu = 2 okuma) cozulur ve
 * onbellege eklenir. Linux'taki karsiligi dcache'tir. */
#include <stdio.h>
#include <string.h>

#define KOK 1                 /* kok dizinin i-node'u */
#define ONB_MAX 16

/* yol -> i-node onbellegi */
static struct { char yol[64]; int ino; } onb[ONB_MAX] = {
    { "/usr", 6 }, { "/usr/ast", 26 }, { "/usr/ast/mbox", 60 },
    { "/usr/bal", 45 }, { "/usr/bal/paper.ps", 85 },
};
static int onb_n = 5;

/* diskteki dizinler: (dizin i-node, ad) -> i-node */
static const struct { int dizin; const char *ad; int ino; } disk[] = {
    { KOK, "usr", 6 }, { KOK, "etc", 14 }, { 6, "ast", 26 }, { 6, "bal", 45 },
    { 26, "mbox", 60 }, { 45, "paper.ps", 85 }, { 45, "tez.tex", 131 },
    { 14, "passwd", 212 },
};

static int son_okuma;         /* son cozumlemenin disk okuma sayisi */

static void yol_kur(char *bil[], int k, char *yol, size_t boy)
{
    size_t u = 0;
    yol[0] = '\0';
    for (int i = 0; i < k && u < boy; i++)
        u += (size_t)snprintf(yol + u, boy - u, "/%s", bil[i]);
}

static int onbellekte(char *bil[], int k, int *ino)
{
    char yol[64];
    yol_kur(bil, k, yol, sizeof yol);
    for (int i = 0; i < onb_n; i++)
        if (strcmp(onb[i].yol, yol) == 0) {
            *ino = onb[i].ino;
            return 1;
        }
    return 0;
}

static void onbellege_ekle(char *bil[], int k, int ino)
{
    if (onb_n == ONB_MAX)
        return;
    yol_kur(bil, k, onb[onb_n].yol, sizeof onb[onb_n].yol);
    onb[onb_n].ino = ino;
    onb_n++;
}

static int dizinde_ara(int dizin, const char *ad)
{
    for (size_t i = 0; i < sizeof disk / sizeof disk[0]; i++)
        if (disk[i].dizin == dizin && strcmp(disk[i].ad, ad) == 0)
            return disk[i].ino;
    return -1;
}

/* bil[]: "usr","bal","tez.tex"; n = 3 */
int cozumle(char *bil[], int n)
{
    int k = n, ino = KOK, okuma = 0;
    while (k > 0 && !onbellekte(bil, k, &ino))
        k--;              /* en uzun onek */
    for (int j = k; j < n; j++) {
        ino = dizinde_ara(ino, bil[j]);
        okuma += 2;       /* i-node + blok */
        onbellege_ekle(bil, j + 1, ino);
    }
    son_okuma = okuma;
    return ino;
}

static void coz(char *bil[], int n)
{
    char yol[64];
    yol_kur(bil, n, yol, sizeof yol);
    int ino = cozumle(bil, n);
    printf("%s -> %d (%d okuma)\n", yol, ino, son_okuma);
}

int main(void)
{
    char *tez[] = { "usr", "bal", "tez.tex" };
    char *passwd[] = { "etc", "passwd" };
    coz(tez, 3);
    coz(tez, 3);              /* ayni yol yeniden: tam yol onbellekte */
    coz(passwd, 2);
    return 0;
}

/* Beklenen cikti:
 * /usr/bal/tez.tex -> 131 (2 okuma)
 * /usr/bal/tez.tex -> 131 (0 okuma)
 * /etc/passwd -> 212 (4 okuma)
 */
