/* Linux'ta PCB: task_struct. Cekirdekteki yapinin kucuk bir kullanici alani
 * benzetimi: ayni alan adlari, cekirdegin cift bagli list_head listesi ve
 * container_of ile bir ebeveynin cocuklari dolasilir. */
#include <stdio.h>
#include <stddef.h>
#include <sys/types.h>

/* ---- <linux/list.h> taklidi ---- */
struct list_head { struct list_head *next, *prev; };

#define container_of(ptr, type, member) \
    ((type *)(void *)((char *)(ptr) - offsetof(type, member)))

static void INIT_LIST_HEAD(struct list_head *h)
{
    h->next = h;
    h->prev = h;
}

static void list_add_tail(struct list_head *yeni, struct list_head *bas)
{
    yeni->prev = bas->prev;
    yeni->next = bas;
    bas->prev->next = yeni;
    bas->prev = yeni;
}

/* ---- cekirdek turlerinin taklitleri ---- */
struct sched_entity { unsigned long long vruntime; };
struct files_struct { int acik_dosya; };
struct mm_struct    { unsigned long toplam_sayfa; };

#define TASK_RUNNING         0x0000
#define TASK_INTERRUPTIBLE   0x0001

struct task_struct {          /* <linux/sched.h> */
    pid_t pid;                     /* surec kimligi */
    unsigned int __state;          /* surec durumu */
    int prio;                      /* oncelik */
    struct sched_entity se;        /* CFS/EEVDF bilgisi */
    struct task_struct *parent;    /* ebeveyn surec */
    struct list_head children;     /* cocuklar */
    struct list_head sibling;      /* ebeveynin children listesindeki halka */
    struct files_struct *files;    /* acik dosyalar */
    struct mm_struct *mm;          /* adres uzayi */
    const char *comm;              /* program adi */
    /* ... yuzlerce alan daha ... */
};

static void gorev_olustur(struct task_struct *t, pid_t pid, const char *comm,
                          struct task_struct *parent, struct files_struct *f,
                          struct mm_struct *mm)
{
    t->pid = pid;
    t->__state = TASK_INTERRUPTIBLE;
    t->prio = 120;                 /* nice 0 */
    t->se.vruntime = 0;
    t->parent = parent;
    INIT_LIST_HEAD(&t->children);
    INIT_LIST_HEAD(&t->sibling);
    t->files = f;
    t->mm = mm;
    t->comm = comm;
    if (parent)
        list_add_tail(&t->sibling, &parent->children);
}

int main(void)
{
    static struct files_struct f1 = {3}, f2 = {5}, f3 = {4};
    static struct mm_struct m1 = {120}, m2 = {800}, m3 = {300};
    struct task_struct init_t, bash_t, vim_t, gcc_t;

    gorev_olustur(&init_t, 1, "systemd", NULL, &f1, &m1);
    gorev_olustur(&bash_t, 2105, "bash", &init_t, &f2, &m2);
    gorev_olustur(&vim_t, 2241, "vim", &bash_t, &f3, &m3);
    gorev_olustur(&gcc_t, 2242, "gcc", &bash_t, &f3, &m3);
    gcc_t.__state = TASK_RUNNING;

    printf("sizeof(struct task_struct) = %zu bayt (gercek cekirdekte ~10 KB)\n",
           sizeof(struct task_struct));
    printf("bash (pid %d), ebeveyni %s (pid %d)\n",
           (int)bash_t.pid, bash_t.parent->comm, (int)bash_t.parent->pid);

    /* list_for_each_entry(cocuk, &bash_t.children, sibling) karsiligi */
    for (struct list_head *p = bash_t.children.next; p != &bash_t.children; p = p->next) {
        struct task_struct *cocuk = container_of(p, struct task_struct, sibling);
        printf("  cocuk pid=%d comm=%s durum=%s prio=%d acik dosya=%d\n",
               (int)cocuk->pid, cocuk->comm,
               cocuk->__state == TASK_RUNNING ? "R" : "S",
               cocuk->prio, cocuk->files->acik_dosya);
    }
    return 0;
}

/* Beklenen cikti (yapi boyutu derleyiciye gore degisebilir; x86-64'te 88):
 * sizeof(struct task_struct) = 88 bayt (gercek cekirdekte ~10 KB)
 * bash (pid 2105), ebeveyni systemd (pid 1)
 *   cocuk pid=2241 comm=vim durum=S prio=120 acik dosya=4
 *   cocuk pid=2242 comm=gcc durum=R prio=120 acik dosya=4
 */
