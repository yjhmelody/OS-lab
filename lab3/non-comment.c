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

static int yjh_open(struct inode *inode, struct file *file)
{
    printk("char_dev device open.\n");
    file->private_data = container_of(inode->i_cdev, yjh_mem_dev, cdev);
    return 0;
}

static ssize_t yjh_read(struct file *file, char __user *buf, size_t n, loff_t *loff)
{
    printk("char_dev read\n");
    unsigned long offset = *loff;
    unsigned long count = device_size - offset >= n ? n : device_size - offset;
    yjh_mem_dev *dev = (yjh_mem_dev *)file->private_data;
    int ret = copy_to_user(buf, dev->mem + offset, count);
    if (ret)
    {
        return ret;
    }
    *loff += count;
    return count;
}
static ssize_t yjh_write(struct file *file, const char __user *buf, size_t n, loff_t *loff)
{
    printk("char_dev write\n");
    unsigned long offset = *loff;
    unsigned long count = device_size - offset >= n ? n : device_size - offset;
    yjh_mem_dev *dev = (yjh_mem_dev *)file->private_data;
    int ret = copy_from_user(dev->mem + offset, buf, count);
    if (ret)
    {
        return ret;
    }
    *loff += count;
    return count;
}
static loff_t yjh_llseek(struct file *file, loff_t offset, int whence)
{
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

static struct file_operations yjh_file_op = {
    .open = yjh_open,
    .read = yjh_read,
    .write = yjh_write,
    .llseek = yjh_llseek,
    .release = yjh_release};

static int
yjh_driver_init(void)
{
    memset(&yjh_dev, 0, sizeof(yjh_dev));
    int ret = register_chrdev_region(MKDEV(device_major, 0), device_count, "yjh_char_device");
    if (ret != 0)
    {
        printk("device number error!\n");
        return ret;
    }
    cdev_init(&yjh_dev.cdev, &yjh_file_op);
    yjh_dev.cdev.owner = THIS_MODULE;
    cdev_add(&yjh_dev.cdev, MKDEV(device_major, 0), device_count);
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
