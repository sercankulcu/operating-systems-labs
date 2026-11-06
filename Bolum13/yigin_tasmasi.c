/* Yigin tampon tasmasi: strcpy uzunluk denetlemedigi icin fazla baytlar
 * cercevedeki komsu hucrelere, en sonunda donus adresine yazilir.
 * Gercek tasma tanimsiz davranis oldugundan cerceve bir bayt dizisiyle
 * benzetilir: [0..7] buffer, [8..15] kayitli rbp, [16..23] donus adresi.
 * Calistirma: ./prog [girdi]   (girdi verilmezse iki ornek kullanilir) */
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define CERCEVE 24
static unsigned char cerceve[CERCEVE + 8];   /* + 8: benzetimin kendi payi */
static const uint64_t DONUS = 0x401196;     /* cagiranin donus adresi */
static const uint64_t RBP = 0x7ffd1000;

static void cerceve_yaz(const char *baslik)
{
    uint64_t rbp, donus;
    memcpy(&rbp, cerceve + 8, 8);
    memcpy(&donus, cerceve + 16, 8);
    printf("  %-8s buffer=\"", baslik);
    for (int i = 0; i < 8; i++)
        putchar(cerceve[i] >= 32 && cerceve[i] < 127 ? cerceve[i] : '.');
    printf("\" rbp=0x%llx donus=0x%llx\n",
           (unsigned long long)rbp, (unsigned long long)donus);
}

/* kucuk, kasitli olarak hatali ornek */
void A(char *in)
{
    char *buffer = (char *)cerceve;      /* char buffer[8]; */
    memset(cerceve, 0, sizeof cerceve);
    memcpy(cerceve + 8, &RBP, 8);        /* cagri: rbp ve donus adresi */
    memcpy(cerceve + 16, &DONUS, 8);
    cerceve_yaz("once:");

    strcpy(buffer, in);   /* sinir denetimi YOK */

    cerceve_yaz("sonra:");
    uint64_t donus;
    memcpy(&donus, cerceve + 16, 8);
    if (donus == DONUS)
        printf("  return: 0x%llx adresine donulur\n", (unsigned long long)donus);
    else
        printf("  return: CPU bozuk 0x%llx adresine atlar, akis ele gecirildi\n",
               (unsigned long long)donus);
    return;
}

int main(int argc, char *argv[])
{
    char kisa[] = "merhaba";
    char uzun[] = "AAAAAAAABBBBBBBBCCCCCC";

    if (argc > 1) {
        if (strlen(argv[1]) >= CERCEVE + 8) {
            printf("benzetim icin girdi %d bayttan kisa olmali\n", CERCEVE + 8);
            return 0;
        }
        A(argv[1]);           /* in: kullanici girdisi */
        return 0;
    }
    printf("girdi \"%s\" (%zu bayt):\n", kisa, strlen(kisa));
    A(kisa);
    printf("girdi \"%s\" (%zu bayt):\n", uzun, strlen(uzun));
    A(uzun);
    return 0;
}

/* Beklenen cikti (x86-64, kucuk sonlu):
 * girdi "merhaba" (7 bayt):
 *   once:    buffer="........" rbp=0x7ffd1000 donus=0x401196
 *   sonra:   buffer="merhaba." rbp=0x7ffd1000 donus=0x401196
 *   return: 0x401196 adresine donulur
 * girdi "AAAAAAAABBBBBBBBCCCCCC" (22 bayt):
 *   once:    buffer="........" rbp=0x7ffd1000 donus=0x401196
 *   sonra:   buffer="AAAAAAAA" rbp=0x4242424242424242 donus=0x434343434343
 *   return: CPU bozuk 0x434343434343 adresine atlar, akis ele gecirildi
 */
