/* Kapi eslemeli ve bellek eslemeli G/C: kapi eslemeli yazmaclara ozel
 * komutlarla (in/out -> Linux'ta inb/outb), bellek eslemeli yazmaclara
 * siradan yukleme/saklama ile (ioremap + readl/writel) erisilir.
 * Gercek erisim root ve donanim gerektirdigi icin kapi alani ve MMIO
 * bolgesi kullanici alaninda dizilerle benzetilir. */
#include <stdio.h>
#include <stdint.h>

/* --- kapi alani (ayri 64 KiB adres uzayi) --- */
static uint8_t kapi_alani[0x10000];

static uint8_t inb(uint16_t port)            /* in  al, dx */
{
    return kapi_alani[port];
}

static void outb(uint8_t deger, uint16_t port) /* out dx, al */
{
    kapi_alani[port] = deger;
}

/* --- bellek eslemeli aygit: fiziksel 0xfebf0000'da 4 KiB yazmac bolgesi --- */
static uint32_t aygit_yazmaclari[1024];   /* 4096 bayt */
#define AYGIT_FIZIKSEL 0xfebf0000UL

static volatile void *ioremap(unsigned long fiziksel, size_t boy)
{
    (void)boy;
    if (fiziksel != AYGIT_FIZIKSEL)
        return NULL;
    return aygit_yazmaclari;          /* gercekte sayfa tablosuna onbelleksiz eslenir */
}

static uint32_t readl(const volatile void *adr)
{
    return *(const volatile uint32_t *)adr;   /* mov eax, [rbx+0x14] */
}

static void writel(uint32_t deger, volatile void *adr)
{
    *(volatile uint32_t *)adr = deger;
}

int main(void)
{
    /* kapi: klavye denetleyicisinin veri kapisi 0x60 */
    outb(0x1c, 0x60);                 /* benzetim: aygit "A" tusu tarama kodunu koydu */
    uint8_t al = inb(0x60);           /* in al, 0x60 */
    printf("inb(0x60) = 0x%02x\n", al);

    /* MMIO: yazmac bolgesi eslenir, 0x14'teki yazmac siradan yukleme ile okunur */
    volatile uint8_t *rbx = ioremap(AYGIT_FIZIKSEL, 4096);
    if (!rbx)
        return 1;
    writel(0xcafe0001u, rbx + 0x14);
    uint32_t eax = readl(rbx + 0x14); /* mov eax, [rbx+0x14] */
    printf("readl(rbx + 0x14) = 0x%08x\n", (unsigned)eax);
    return 0;
}

/* Beklenen cikti:
 * inb(0x60) = 0x1c
 * readl(rbx + 0x14) = 0xcafe0001
 * Gercek sistemde kapilar /proc/ioports'ta, MMIO bolgeleri /proc/iomem'de
 * listelenir.
 */
