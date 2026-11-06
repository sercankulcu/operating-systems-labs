/* Diffie-Hellman anahtar degisimi: kare al ve carp yontemiyle modpow.
 * Acik p = 23, g = 5; Alice'in gizli a = 6, Bob'un gizli b = 15.
 * Ikisi de ayni ortak sirri (5^(a*b) mod 23) hesaplar. */
#include <stdio.h>

long modpow(long g, long e, long p)
{
    long r = 1;
    while (e > 0) {
        if (e & 1) r = r * g % p;
        g = g * g % p;
        e >>= 1;
    }
    return r;
}

int main(void)
{
    /* acik: p = 23, g = 5 */
    long A  = modpow(5, 6, 23);
    long B  = modpow(5, 15, 23);
    long sA = modpow(B, 6, 23);
    long sB = modpow(A, 15, 23);
    printf("A = %ld\n", A);
    printf("B = %ld\n", B);
    printf("sA = %ld\n", sA);
    printf("sB = %ld\n", sB);
    return 0;
}

/* Beklenen cikti:
 * A = 8
 * B = 19
 * sA = 2
 * sB = 2
 */
