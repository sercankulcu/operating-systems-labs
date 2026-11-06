/* Biriktirme (spooling) kuyrugu: programlar dosyalari 4 yuvali dairesel
 * kuyruga ekler, biriktirme hizmeti onlari sirayla yaziciya gonderir. */
#include <stdio.h>
#include <stddef.h>

char *kuyruk[4];
int giris = 0, cikis = 0, adet = 0;

/* Yazici benzetimi: dosya adini ekrana yazar. */
void yazici_gonder(char *dosya)
{
    printf("%s\n", dosya);
}

void ekle(char *dosya)     /* programlar */
{
    kuyruk[giris] = dosya;
    giris = (giris + 1) % 4;
    adet++;
}

void biriktirme_hizmeti(void)
{
    while (adet > 0) {
        yazici_gonder(kuyruk[cikis]);
        kuyruk[cikis] = NULL;   /* yuva bosalir */
        cikis = (cikis + 1) % 4;
        adet--;
    }
}

static void durum_yaz(const char *olay)
{
    printf("[%s] giris=%d cikis=%d adet=%d\n", olay, giris, cikis, adet);
}

int main(void)
{
    durum_yaz("baslangic");
    ekle("a.pdf");
    ekle("b.txt");
    ekle("c.doc");
    ekle("d.png");
    durum_yaz("4 ekle");
    biriktirme_hizmeti();
    durum_yaz("hizmet");
    return 0;
}

/* Beklenen cikti:
 * [baslangic] giris=0 cikis=0 adet=0
 * [4 ekle] giris=0 cikis=0 adet=4
 * a.pdf
 * b.txt
 * c.doc
 * d.png
 * [hizmet] giris=0 cikis=0 adet=0
 */
