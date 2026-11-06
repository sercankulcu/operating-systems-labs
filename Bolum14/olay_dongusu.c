/* Yurutme paradigmalari (b) olay gudumlu kod: akisi dis olaylar belirler.
 * get_message siradaki olayi verir, switch onu isleyiciye yonlendirir;
 * QUIT gelince 0 doner ve dongu biter. Olay kuyrugu bir dizi ile benzetilir
 * (gercekte bos kuyrukta get_message sureci bloke eder). */
#include <stdio.h>

enum { QUIT = 0, KEY = 1, CLICK = 2, TIMER = 3 };   /* case 1, 2, 3 */

typedef struct { int type; int veri; } msg_t;

static msg_t kuyruk[8];
static int bas = 0, son = 0;

static void olay_gelir(int type, int veri)
{
    kuyruk[son].type = type;
    kuyruk[son].veri = veri;
    son++;
}

static void init(void)
{
    printf("init: pencere hazir, kuyruk bos\n");
    /* kullanici tiklar, tusa basar, zamanlayici dolar, sonra pencereyi kapatir */
    olay_gelir(CLICK, 0);
    olay_gelir(KEY, 'q');
    olay_gelir(TIMER, 500);
    olay_gelir(QUIT, 0);
}

static int get_message(msg_t *m)
{
    if (bas == son)
        return 0;               /* benzetim: bos kuyrukta bloke olmak yerine bitir */
    *m = kuyruk[bas++];
    return m->type != QUIT;     /* QUIT icin 0 */
}

static void on_key(msg_t *m)   { printf("on_key: '%c'\n", m->veri); }
static void on_click(msg_t *m) { (void)m; printf("on_click\n"); }
static void on_timer(msg_t *m) { printf("on_timer: %d ms, imlec yanip soner\n", m->veri); }

int main(void)
{
    msg_t m;
    init();
    while (get_message(&m)) {
        switch (m.type) {
        case KEY:   on_key(&m);   break;
        case CLICK: on_click(&m); break;
        case TIMER: on_timer(&m); break;
        }
    }
    printf("QUIT: dongu bitti\n");
    return 0;
}

/* Beklenen cikti:
 * init: pencere hazir, kuyruk bos
 * on_click
 * on_key: 'q'
 * on_timer: 500 ms, imlec yanip soner
 * QUIT: dongu bitti
 */
