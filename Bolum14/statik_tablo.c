/* Statik ve dinamik yapilar: sabit boyutlu surec tablosunda isaretciyle
 * dogrusal arama. p++ bir sonraki yapiya ilerler; bos girisler (pid 0) de
 * taranir. En kotu durumda tum tablo: O(n). */
#include <stdio.h>

#define PROC_TABLE_SIZE 6

struct proc {
    int proc_pid;            /* 0: bos giris */
};

struct proc proc_table[PROC_TABLE_SIZE] = {
    { 1 }, { 314 }, { 0 }, { 42 }, { 977 }, { 0 }
};

static int ara(int pid)
{
    struct proc *p;
    int found, bakilan = 0;

    found = 0;
    for (p = &proc_table[0];
         p < &proc_table[PROC_TABLE_SIZE];
         p++) {
        bakilan++;
        if (p->proc_pid == pid) {
            found = 1;
            break;
        }
    }
    printf("pid %d: found = %d, %d giris tarandi", pid, found, bakilan);
    if (found)
        printf(", proc_table[%td]", p - proc_table);
    printf("\n");
    return found;
}

int main(void)
{
    ara(42);
    ara(7);                  /* yok: tum tablo taranir */
    return 0;
}

/* Beklenen cikti:
 * pid 42: found = 1, 4 giris tarandi, proc_table[3]
 * pid 7: found = 0, 6 giris tarandi
 */
