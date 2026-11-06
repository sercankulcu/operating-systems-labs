/* TOTP (RFC 6238): zaman 30 saniyelik adimlara bolunur, adim sayisi gizli
 * anahtarla HMAC-SHA1'den gecer, dinamik kesme ile 6 haneli kod uretilir.
 * SHA-1 ve HMAC kucuk bir uygulama olarak dosyada yazilidir. */
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define ROTL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

/* kisa iletiler icin SHA-1 (en cok 200 bayt) */
static void sha1(const uint8_t *msg, size_t len, uint8_t out[20])
{
    uint8_t b[256];
    size_t n = len;
    uint32_t h0 = 0x67452301, h1 = 0xEFCDAB89, h2 = 0x98BADCFE,
             h3 = 0x10325476, h4 = 0xC3D2E1F0;

    memcpy(b, msg, len);
    b[n++] = 0x80;
    while (n % 64 != 56)
        b[n++] = 0;
    uint64_t bitler = (uint64_t)len * 8;
    for (int i = 7; i >= 0; i--)
        b[n++] = (uint8_t)(bitler >> (8 * i));

    for (size_t blok = 0; blok < n; blok += 64) {
        uint32_t w[80];
        for (int t = 0; t < 16; t++)
            w[t] = (uint32_t)b[blok + 4 * t] << 24 | (uint32_t)b[blok + 4 * t + 1] << 16 |
                   (uint32_t)b[blok + 4 * t + 2] << 8 | (uint32_t)b[blok + 4 * t + 3];
        for (int t = 16; t < 80; t++)
            w[t] = ROTL(w[t - 3] ^ w[t - 8] ^ w[t - 14] ^ w[t - 16], 1);

        uint32_t a = h0, bb = h1, c = h2, d = h3, e = h4;
        for (int t = 0; t < 80; t++) {
            uint32_t f, k;
            if (t < 20)      { f = (bb & c) | (~bb & d);           k = 0x5A827999; }
            else if (t < 40) { f = bb ^ c ^ d;                     k = 0x6ED9EBA1; }
            else if (t < 60) { f = (bb & c) | (bb & d) | (c & d);  k = 0x8F1BBCDC; }
            else             { f = bb ^ c ^ d;                     k = 0xCA62C1D6; }
            uint32_t tmp = ROTL(a, 5) + f + e + k + w[t];
            e = d; d = c; c = ROTL(bb, 30); bb = a; a = tmp;
        }
        h0 += a; h1 += bb; h2 += c; h3 += d; h4 += e;
    }

    uint32_t h[5] = { h0, h1, h2, h3, h4 };
    for (int i = 0; i < 5; i++) {
        out[4 * i]     = (uint8_t)(h[i] >> 24);
        out[4 * i + 1] = (uint8_t)(h[i] >> 16);
        out[4 * i + 2] = (uint8_t)(h[i] >> 8);
        out[4 * i + 3] = (uint8_t)h[i];
    }
}

/* HMAC-SHA1(K, T): K 20 baytlik anahtar, T 8 baytlik buyuk sonlu sayi */
static void hmac_sha1(const uint8_t *K, uint64_t T, uint8_t h[20])
{
    uint8_t ic[64 + 8], dis[64 + 20];
    for (int i = 0; i < 64; i++) {
        uint8_t k = i < 20 ? K[i] : 0;
        ic[i] = k ^ 0x36;
        dis[i] = k ^ 0x5c;
    }
    for (int i = 0; i < 8; i++)
        ic[64 + i] = (uint8_t)(T >> (8 * (7 - i)));
    sha1(ic, sizeof ic, dis + 64);
    sha1(dis, sizeof dis, h);
}

uint32_t totp(uint8_t *K, uint64_t t)
{
    uint64_t T = t / 30;    /* 30 s */
    uint8_t h[20];
    hmac_sha1(K, T, h);
    int o = h[19] & 0x0F;   /* bagil konum */
    uint32_t v = (uint32_t)(h[o] & 0x7F) << 24
               | (uint32_t)h[o+1] << 16
               | (uint32_t)h[o+2] << 8
               | (uint32_t)h[o+3];
    return v % 1000000;     /* 6 hane */
}

int main(void)
{
    uint8_t K[20];
    memcpy(K, "12345678901234567890", 20);   /* RFC 6238 test anahtari */
    uint64_t anlar[] = { 59, 75 };
    for (int i = 0; i < 2; i++)
        printf("t=%llu: %06u\n", (unsigned long long)anlar[i], (unsigned)totp(K, anlar[i]));
    return 0;
}

/* Beklenen cikti:
 * t=59: 287082
 * t=75: 359152
 * (t=59 icin RFC 6238'in 8 haneli test degeri 94287082'dir; son 6 hanesi) */
