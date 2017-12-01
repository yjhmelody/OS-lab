#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <asm/io.h>

// http://www.linuxidc.com/Linux/2016-02/128601.htm
MODULE_LICENSE("GPL");

// const static int device_size = 512;
#define device_size 512
const static dev_t device_first = 1000;
const static unsigned int device_count = 10;

// yjh's char driver
typedef struct yjh_mem_dev
{
    struct cdev cdev;
    unsigned char mem[device_size];
} yjh_mem_dev;

yjh_mem_dev yjh_dev;

// #define container_of(ptr, type, member)
// 该宏的功能是计算返回包含 ptr 指向的成员所在的 type 类型数据结构的指针，其实现思路是：计算 type 结构体成员 member 在结构体中的偏移量，然后用
// ptr 的值减去这个偏移量，就得出 type 数据结构的首地址。

// 这些函数具体实现设备的相关操作
static int yjh_open(struct inode *inode, struct file *file)
{
    // private_data 存储yjh_mem_dev
    file->private_data = container_of(inode->i_cdev, yjh_mem_dev, cdev);
    printk("char_dev device open.\n");
    return 0;
}

static ssize_t yjh_read(struct file *file, char __user *buf, size_t n, loff_t *loff)
{
    // 当前偏移量
    unsigned long offset = *loff;
    // 实际需要读写的数量
    unsigned long count = device_size - offset >= n ? n : device_size - offset;
    yjh_mem_dev *dev = (yjh_mem_dev *)file->private_data;
    int ret = copy_to_user(buf, dev->mem + offset, count);
    // 把错误返回给上层处理
    if (ret)
    {
        return ret;
    }
    // 读成功后修改file下的loff
    *loff += count;
    printk("char_dev read\n");
    return count;
}
static ssize_t yjh_write(struct file *file, const char __user *buf, size_t n, loff_t *loff)
{
    // 当前偏移量
    unsigned long offset = *loff;
    // 实际需要读写的数量
    unsigned long count = device_size - offset >= n ? n : device_size - offset;
    yjh_mem_dev *dev = (yjh_mem_dev *)file->private_data;
    int ret = copy_from_user(dev->mem + offset, buf, count);
    // 把错误返回给上层处理
    if (ret)
    {
        return ret;
    }
    // 读成功后修改file下的loff
    *loff += count;
    printk("char_dev write\n");
    return count;
}
// static int yjh_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
// {
//     printk("char_dev device ioctl.\n");
//     return 0;
// }
static int yjh_release(struct inode *inode, struct file *file)
{
    printk("char_dev device resealed.\n");
    return 0;
}

// file_operations 结构体中包含很多函数指针，是驱动程序与内核的接口
static struct file_operations yjh_file_op = {
    // .owner = THIS_MODLUE,
    .open = yjh_open,
    .read = yjh_read,
    .write = yjh_write,
    // .ioctl = yjh_ioctl,
    .release = yjh_release};

static int
yjh_driver_init(void)
{
    // device init
    memset(&yjh_dev, 0, sizeof(yjh_dev));
    cdev_init(&yjh_dev.cdev, &yjh_file_op);
    int ret = cdev_add(&yjh_dev.cdev, device_first, device_count);
    if (ret < 0)
    {
        printk("device number error!\n");
    }
}

static void
yjh_driver_exit(void)
{
    cdev_del(&yjh_dev.cdev);
    // 动态分配设备号 int alloc_chrdev_region(dev_t *dev,unsigned firstminor,unsigned count,char *name)
    // 静态分配设备号
    unregister_chrdev_region(device_first, device_count);
}

module_init(yjh_driver_init);
module_exit(yjh_driver_exit);