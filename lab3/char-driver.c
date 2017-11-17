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

// my char driver
typedef struct yjh_mem_dev
{
    struct cdev cdev;
    unsigned char mem[512];
} yjh_mem_dev;

static int driver_init(void)
{

}

static void driver_exit(void)
{
    printk(KERN_ALERT "good bye char driver\n");
}

module_init(hello_init);
module_exit(hello_exit);