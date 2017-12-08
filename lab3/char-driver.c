#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/init_task.h>
#include <asm/uaccess.h>
// http://www.linuxidc.com/Linux/2016-02/128601.htm
MODULE_LICENSE("GPL");

// const static int device_size = 512;
#define device_size 512
const static dev_t device_major = 1000;
const static unsigned int device_count = 1;

// yjh's char driver
typedef struct yjh_mem_dev
{
    struct cdev cdev;
    unsigned char mem[device_size];
} yjh_mem_dev;

yjh_mem_dev yjh_dev;

// 这些函数具体实现设备的相关操作
static int yjh_open(struct inode *inode, struct file *file)
{
    printk("char_dev device open.\n");
    // #define container_of(ptr, type, member)
    // 该宏的功能是计算返回包含 ptr 指向的成员所在的 type 类型数据结构的指针，其实现思路是：计算 type 结构体成员 member 在结构体中的偏移量，然后用
    // ptr 的值减去这个偏移量，就得出 type 数据结构的首地址。
    // private_data 存储yjh_mem_dev
    file->private_data = container_of(inode->i_cdev, yjh_mem_dev, cdev);
    return 0;
}

static ssize_t yjh_read(struct file *file, char __user *buf, size_t n, loff_t *loff)
{
    printk("char_dev read\n");
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
    return count;
}
static ssize_t yjh_write(struct file *file, const char __user *buf, size_t n, loff_t *loff)
{
    printk("char_dev write\n");
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
    return count;
}
// static int yjh_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
// {
//     printk("char_dev device ioctl.\n");
//     return 0;
// }

// 参数说明
// fd:文件描述符
// offset:字节偏移量
// whence:下述常数之一
// SEEK_SET将新位置指定成从文件起始处的一个偏移距离
// SEEK_CUR将新位置指定成从当前位置开始的一个偏移距离
// SEEK_END将新位置指定成从文件结尾开始的的一个偏移距离
// 返回一个新位置，设置成从文件起始处算起的一个偏移量。-1表示函数执行出错。会设置errno
static loff_t yjh_llseek(struct file *file, loff_t offset, int whence)
{
    // SEEK_SET 
    if (whence != SEEK_SET) {
        return -1;
    }
    if (offset < 0 || offset > 512) {
        return -1;
    }
    filp->f_pos = offset;
    return offset;
}

static int yjh_release(struct inode *inode, struct file *file)
{
    printk("char_dev device release.\n");
    return 0;
}

// file_operations 结构体中包含很多函数指针，是驱动程序与内核的接口
static struct file_operations yjh_file_op = {
    // .owner = THIS_MODLUE,
    .open = yjh_open,
    .read = yjh_read,
    .write = yjh_write,
    // .ioctl = yjh_ioctl,
    .llseek = yjh_llseek,
    .release = yjh_release};

// int register_chrdev_region(dev_t first, unsigned int count, char *name);
// 函数功能：为一个指定主设备号的字符驱动程序申请一个或一组连续的次设备号。
// 输入参数：
// first ：dev_t 类型的起始设备号（可通过 MKDEV(major,0)获得）；
// count：需要申请的次设备号数量；
// name：设备名，会出现在 /proc/devices 和 sysfs 中；
// 返回值：分配成功返回 0，失败返回一个负的错误码
static int
yjh_driver_init(void)
{
    // device init
    memset(&yjh_dev, 0, sizeof(yjh_dev));
    // 动态分配设备号 int alloc_chrdev_region(dev_t *dev,unsigned firstminor,unsigned count,char *name)
    // 静态分配设备号 int register_chrdev_region(dev_t first, unsigned int count, char *name)
    // 注册设备号
    int ret = register_chrdev_region(MKDEV(device_major, 0), device_count, "yjh_char_device");
    if (ret != 0)
    {
        printk("device number error!\n");
        return ret;
    }
    // 初始化cdev 并赋值上file操作的函数指针
    cdev_init(&yjh_dev.cdev, &yjh_file_op);
    yjh_dev.cdev.owner = THIS_MODULE;
    // 注册 cdev 结构
    cdev_add(&yjh_dev.cdev, MKDEV(device_major, 0), device_count);
    // 执行 cdev_add()操作后，意味着一个字符设备对象已经加入了系统，以后用户程序可
    // 以通过文件系统接口找到对应的驱动程序。

    printk("init\n");
    return 0;
}

static void
yjh_driver_exit(void)
{
    cdev_del(&yjh_dev.cdev);
    // void unregister_chrdev_region(dev_t from, unsigned count);
    unregister_chrdev_region(device_major, device_count);
    printk("device is released!\n");
}

module_init(yjh_driver_init);
module_exit(yjh_driver_exit);
