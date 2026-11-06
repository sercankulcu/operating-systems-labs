/* Aygittan bagimsiz G/C yazilimi: VFS her cagriyi surucunun file_operations
 * tablosundaki fonksiyona yonlendirir. Cekirdek turleri (inode, file, kiocb,
 * iov_iter) kullanici alaninda basit taklitlerle benzetilir. */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

/* --- cekirdek turlerinin taklitleri --- */
struct file_operations;
struct inode { int i_rdev_major, i_rdev_minor; };
struct file {
    const struct file_operations *f_op;
    long long f_pos;
    void *private_data;
};
struct kiocb { struct file *ki_filp; long long ki_pos; };
struct iov_iter { char *buf; size_t count; };
typedef unsigned int poll_mask_t;
#define EPOLLIN 0x001
#define NVME_IOCTL_ID 0x4e40   /* gercek degerden bagimsiz ornek komut */

struct file_operations {
    int      (*open)(struct inode *, struct file *);
    int      (*release)(struct inode *, struct file *);
    ssize_t  (*read_iter)(struct kiocb *, struct iov_iter *);
    long     (*unlocked_ioctl)(struct file *, unsigned int, unsigned long);
    poll_mask_t (*poll)(struct file *, void *);
};

/* --- NVMe surucusu (benzetim): kucuk bir ad alani --- */
static const char ad_alani[] = "NVMe blok 0: merhaba dunya!....";

static int nvme_open(struct inode *ino, struct file *f)
{
    printf("nvme_open: aygit %d:%d\n", ino->i_rdev_major, ino->i_rdev_minor);
    f->private_data = (void *)ad_alani;
    return 0;
}

static int nvme_release(struct inode *ino, struct file *f)
{
    (void)ino;
    (void)f;
    printf("nvme_release\n");
    return 0;
}

static ssize_t nvme_read(struct kiocb *iocb, struct iov_iter *to)
{
    const char *veri = iocb->ki_filp->private_data;
    size_t boy = sizeof ad_alani - 1;
    if ((size_t)iocb->ki_pos >= boy)
        return 0;
    size_t n = boy - (size_t)iocb->ki_pos;
    if (n > to->count)
        n = to->count;
    memcpy(to->buf, veri + iocb->ki_pos, n);
    iocb->ki_pos += (long long)n;
    return (ssize_t)n;
}

static long nvme_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    (void)f;
    (void)arg;
    if (cmd == NVME_IOCTL_ID)
        return 1;                      /* ad alani kimligi */
    return -25;                        /* -ENOTTY */
}

static poll_mask_t nvme_poll(struct file *f, void *wait)
{
    (void)f;
    (void)wait;
    return EPOLLIN;                    /* blok aygiti her zaman okunabilir */
}

struct file_operations nvme_fops = {
    .open           = nvme_open,
    .release        = nvme_release,
    .read_iter      = nvme_read,
    .unlocked_ioctl = nvme_ioctl,
    .poll           = nvme_poll,
};

/* --- VFS: aygittan bagimsiz katman, yalniz f_op'u cagirir --- */
static int vfs_open(struct inode *ino, struct file *f,
                    const struct file_operations *fops)
{
    f->f_op = fops;
    f->f_pos = 0;
    return f->f_op->open ? f->f_op->open(ino, f) : 0;
}

static ssize_t vfs_read(struct file *f, char *buf, size_t n)
{
    struct kiocb k = { f, f->f_pos };
    struct iov_iter it = { buf, n };
    ssize_t r = f->f_op->read_iter(&k, &it);
    if (r > 0)
        f->f_pos = k.ki_pos;
    return r;
}

int main(void)
{
    struct inode ino = { 259, 0 };     /* /dev/nvme0n1 -> 259:0 */
    struct file f;
    char buf[16];

    vfs_open(&ino, &f, &nvme_fops);
    ssize_t r = vfs_read(&f, buf, 12);
    printf("read: %zd bayt \"%.*s\"\n", r, (int)r, buf);
    r = vfs_read(&f, buf, 12);
    printf("read: %zd bayt \"%.*s\"\n", r, (int)r, buf);
    printf("ioctl(NVME_IOCTL_ID) = %ld\n", f.f_op->unlocked_ioctl(&f, NVME_IOCTL_ID, 0));
    printf("poll = %s\n", (f.f_op->poll(&f, NULL) & EPOLLIN) ? "EPOLLIN" : "-");
    f.f_op->release(&ino, &f);
    return 0;
}

/* Beklenen cikti:
 * nvme_open: aygit 259:0
 * read: 12 bayt "NVMe blok 0:"
 * read: 12 bayt " merhaba dun"
 * ioctl(NVME_IOCTL_ID) = 1
 * poll = EPOLLIN
 * nvme_release
 */
