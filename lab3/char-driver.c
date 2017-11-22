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

const static int device_size = 512;
const static dev_t first = 1000;
const static unsigned int count = 10;

// yjh's char driver
typedef struct yjh_mem_dev
{
    struct cdev cdev;
    unsigned char mem[device_size];
} yjh_mem_dev;

yjh_mem_dev yjh_dev;

// 这些函数具体实现设备的相关操作
static int (*yjh_open)(struct inode *inode, struct file *file)
{
    file->private_data = contain_of(inode->i_cdev, yjh_mem_dev, cdev);
    return 0;
}

static int (*yjh_read)(struct file *file, char __user *buf, size_t n, loff_t *loff)
{
    yjh_mem_dev *dev = (yjh_mem_dev *)file->private_data;
    copy_to_user(buf, dev, n);
}
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
driver_init()
{
    memset(&yjh_dev, 0, sizeof(yjh_dev));
    cdev_init(&yjh_dev.cdev, yjh_file_op);
    int ret = cdev_add(&yjh_dev.cdev, first, count);
    if (ret < 0)
    {
        printk("device number error!\n");
    }
}

static void driver_exit()
{
    cdev_del(&yjh_dev.cdev);
    // 动态分配设备号 int alloc_chrdev_region(dev_t *dev,unsigned firstminor,unsigned count,char *name)
    // 静态分配设备号
    int ret = unregister_chrdev_region(, count);
    if (ret < 0)
    {
        printk("allocation error!\n");
        return -1;
    }
}

module_init(hello_init);
module_exit(hello_exit);