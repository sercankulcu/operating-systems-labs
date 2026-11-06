/* Dosya izin denetimi: root (euid 0) okuma/yazmada izin bitlerine bakilmaz;
 * digerlerinde ilk eslesen sinif (sahip, grup, diger) secilir ve yalniz o
 * sinifin bitleri denetlenir. cred ve inode yapilari benzetimdir. */
#include <stdio.h>

typedef struct {
    int euid;
    int gid[4];              /* uye olunan gruplar */
    int ngid;
} cred;

typedef struct {
    int mode;                /* rwxrwxrwx bitleri */
    int uid, gid;
} inode;

enum { ALI = 1000, VELI = 1001, CAN = 1002 };
enum { EKIP = 100, DIGER_GRUP = 200 };

static int grupta(cred *c, int gid)
{
    for (int i = 0; i < c->ngid; i++)
        if (c->gid[i] == gid)
            return 1;
    return 0;
}

int izin_var(cred *c, inode *f, int istek)
{
    int bit, m = f->mode;       /* r4 w2 x1 */
    if (c->euid == 0)           /* root */
        return !(istek & 1) || (m & 0111);
    if (c->euid == f->uid)
        bit = (m >> 6) & 7;     /* sahip */
    else if (grupta(c, f->gid))
        bit = (m >> 3) & 7;     /* grup */
    else
        bit = m & 7;            /* diger */
    return (bit & istek) == istek;
}

static void dene(const char *on, const char *ad, cred *c, inode *f,
                 const char *iad, int istek)
{
    printf("%s%s %s: %s\n", on, ad, iad, izin_var(c, f, istek) ? "izin" : "ret");
}

int main(void)
{
    cred ali  = { ALI,  { EKIP }, 1 };
    cred veli = { VELI, { EKIP }, 1 };
    cred can  = { CAN,  { DIGER_GRUP }, 1 };
    cred root = { 0,    { 0 }, 1 };
    inode rapor = { 0640, ALI, EKIP };      /* rapor.txt: rw-r----- ali ekip */

    dene("", "ali", &ali, &rapor, "w", 2);
    dene("", "veli", &veli, &rapor, "w", 2);
    dene("", "veli", &veli, &rapor, "r", 4);
    dene("", "can", &can, &rapor, "r", 4);
    dene("", "root", &root, &rapor, "r", 4);

    rapor.mode = 0066;                      /* ----rw-rw- */
    dene("0066 ", "ali", &ali, &rapor, "r", 4);
    dene("0066 ", "veli", &veli, &rapor, "r", 4);
    return 0;
}

/* Beklenen cikti:
 * ali w: izin
 * veli w: ret
 * veli r: izin
 * can r: ret
 * root r: izin
 * 0066 ali r: ret
 * 0066 veli r: izin
 */
