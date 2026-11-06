/* Olay dongusu: epoll ile tek is parcacigi birden cok bloke etmeyen
 * tanimlayiciyi izler; yalniz hazir olanlar islenir. Soketler yerine yerel
 * soket ciftleri (socketpair) kullanilir; sonsuz dongu sinirli turla calisir. */
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#define NSOCK 3
#define TUR 3

static int s[NSOCK];        /* sunucunun izledigi uclar */
static int karsi[NSOCK];    /* istemci uclari: veri buradan gelir */

static int sira(int fd)
{
    for (int k = 0; k < NSOCK; k++)
        if (s[k] == fd)
            return k;
    return -1;
}

static void handle(int fd, char *buf, ssize_t r)
{
    printf("  s[%d]: %zd bayt okundu, ilk bayt '%c'\n", sira(fd), r, buf[0]);
}

int main(void)
{
    struct epoll_event ev, evs[64];
    char buf[4096];
    int k, nsock = NSOCK;

    for (k = 0; k < nsock; k++) {
        int cift[2];
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, cift) < 0) {
            perror("socketpair");
            return 1;
        }
        s[k] = cift[0];
        karsi[k] = cift[1];
        fcntl(s[k], F_SETFL, fcntl(s[k], F_GETFL) | O_NONBLOCK);
    }

    int ep = epoll_create1(0);
    for (k = 0; k < nsock; k++) {   /* O_NONBLOCK */
        memset(&ev, 0, sizeof ev);
        ev.events = EPOLLIN;
        ev.data.fd = s[k];
        epoll_ctl(ep, EPOLL_CTL_ADD, s[k], &ev);
    }

    for (int tur = 0; tur < TUR; tur++) {   /* for (;;) yerine TUR tur */
        /* benzetim: istemciler veri gonderir */
        if (tur == 0) {
            memset(buf, 'x', 512);
            if (write(karsi[2], buf, 512) != 512)   /* s[2]'ye 512 bayt */
                perror("write");
        } else if (tur == 1) {
            memset(buf, 'y', 100);
            if (write(karsi[0], buf, 100) != 100)   /* s[0]'a 100 bayt */
                perror("write");
        }

        int n = epoll_wait(ep, evs, 64, 200); /* -1 yerine 200 ms */
        printf("tur %d: epoll_wait -> n = %d\n", tur, n);
        for (int i = 0; i < n; i++) {
            int fd = evs[i].data.fd;
            ssize_t r = read(fd, buf, sizeof buf);
            if (r < 0 && errno == EAGAIN) continue;
            handle(fd, buf, r);
            /* ikinci okuma: veri bitti, bloke etmeyen read EAGAIN doner */
            r = read(fd, buf, sizeof buf);
            if (r < 0 && errno == EAGAIN)
                printf("  s[%d]: EAGAIN, beklenecek veri kalmadi\n", sira(fd));
        }
    }

    close(ep);
    for (k = 0; k < nsock; k++) {
        close(s[k]);
        close(karsi[k]);
    }
    return 0;
}

/* Beklenen cikti:
 * tur 0: epoll_wait -> n = 1
 *   s[2]: 512 bayt okundu, ilk bayt 'x'
 *   s[2]: EAGAIN, beklenecek veri kalmadi
 * tur 1: epoll_wait -> n = 1
 *   s[0]: 100 bayt okundu, ilk bayt 'y'
 *   s[0]: EAGAIN, beklenecek veri kalmadi
 * tur 2: epoll_wait -> n = 0
 * (tur 2'de hazir soket yok: 200 ms zaman asimi)
 */
