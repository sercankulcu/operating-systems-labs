/* Dinamik yukleme: libm calisma aninda dlopen ile yuklenir, sqrt dlsym ile bulunur.
 * glibc 2.34'ten beri dlopen libc'dedir; -ldl gerekmez.
 * Derleme: gcc dinamik_yukleme.c -o dinamik_yukleme */
#include <dlfcn.h>
#include <stdio.h>

int main(void)
{
    void *h = dlopen("libm.so.6", RTLD_LAZY);
    if (!h) { puts(dlerror()); return 1; }
    double (*kok)(double) =
        (double (*)(double)) dlsym(h, "sqrt");
    printf("%.3f\n", kok(2.0));
    dlclose(h);
    return 0;
}

/* Beklenen cikti:
 * 1.414
 */
