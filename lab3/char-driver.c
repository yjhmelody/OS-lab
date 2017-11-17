#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init_task.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");

// yjh's char driver
typedef struct yjh_mem_dev
{
    struct cdev cdev;
    unsigned char mem[512];
} yjh_mem_dev;

// 这些函数具体实现设备的相关操作
static int (*yjh_open)(struct inode *inode, struct file *file);
static int (*yjh_read)(struct file *file, char __user *buf, size_t n, loff_t *loff);
static int (*yjh_write)(struct file *file, const char __user *buf, size_t n, loff_t *loff);
static int (*yjh_ioctl)(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static int (*yjh_release)(struct inode *inode, struct file *file);

// file_operations 结构体中包含很多函数指针，是驱动程序与内核的接口
static struct file_operations yjh_file_op = {
    .owner = THIS_MODLUE,
    .open = yjh_open,
    .read = yjh_read,
    .write = yjh_write,
    .ioctl = yjh_ioctl,
    .release = yjh_release}

static int
driver_init(void)
{
}

static void driver_exit(void)
{
    printk(KERN_ALERT "good bye char driver\n");
}

module_init(hello_init);
module_exit(hello_exit);