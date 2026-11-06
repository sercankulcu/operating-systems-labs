/* Erisim denetim listesi (ACL): A dosyasinin ACL'si koruma matrisinin A
 * sutunudur. Karar ilk eslesen girdide verilir; listede olmayan grup icin
 * guvenli varsayilan: yasak. Haklar R=4 W=2 X=1. */
#include <stdio.h>

enum { G1, G2, G3, G4, YON };
#define R 4
#define W 2
#define X 1

/* A dosyasinin ACL'si: R=4 W=2 X=1 */
struct girdi { int grup, hak; } acl[] =
    {{G1, 4}, {G2, 2}, {YON, 7}};

int erisim(int grup, int istek)
{
    for (int i = 0; i < 3; i++)
        if (acl[i].grup == grup) {
            int h = acl[i].hak;
            return (h & istek) == istek;
        }
    return 0;   /* listede yok: yasak */
}

static void dene(const char *gad, int grup, const char *iad, int istek)
{
    printf("%s %s: %s\n", gad, iad, erisim(grup, istek) ? "izin" : "ret");
}

int main(void)
{
    dene("G2", G2, "W", W);
    dene("G1", G1, "W", W);
    dene("G4", G4, "R", R);
    dene("YON", YON, "RX", R | X);
    return 0;
}

/* Beklenen cikti:
 * G2 W: izin
 * G1 W: ret
 * G4 R: ret
 * YON RX: izin
 */
