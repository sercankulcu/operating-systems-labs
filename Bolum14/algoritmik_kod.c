/* Yurutme paradigmalari (a) algoritmik kod: program akisini program
 * belirler ve isletim sistemine bloke eden read/write cagrilari yapar.
 * fd, bir yerel soket ciftinin bir ucudur; karsi uc "dis dunya"yi oynar. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

static int fd;          /* programin konustugu tanimlayici */
static int dis;         /* karsi uc */

static void init(void)
{
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0) {
        perror("socketpair");
        exit(1);
    }
    fd = sv[0];
    dis = sv[1];
    int gelen = 21;
    if (write(dis, &gelen, sizeof gelen) != (ssize_t)sizeof gelen)  /* okunacak veri hazir */
        perror("write");
    printf("init\n");
}

static void do_something(void)
{
    printf("do_something\n");
}

static void do_something_else(void)
{
    printf("do_something_else\n");
}

static void keep_going(void)
{
    int v;
    if (read(dis, &v, sizeof v) == (ssize_t)sizeof v)
        printf("keep_going: karsi taraf y = %d aldi\n", v);
    close(fd);
    close(dis);
}

int main(void)
{
    int x, y;
    init();
    do_something();
    if (read(fd, &x, sizeof x) != (ssize_t)sizeof x)   /* veri gelene dek bloke */
        return 1;
    printf("read: x = %d\n", x);
    do_something_else();
    y = 2 * x;
    if (write(fd, &y, sizeof y) != (ssize_t)sizeof y)
        return 1;
    keep_going();
    exit(0);
}

/* Beklenen cikti:
 * init
 * do_something
 * read: x = 21
 * do_something_else
 * keep_going: karsi taraf y = 42 aldi
 */
