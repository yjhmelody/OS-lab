//#include<linux/fs.h>
//#include<linux/types.h>
#include<linux/cdev.h>
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/slab.h>
//#include<linux/errno.h>
#include<linux/module.h>
#include<linux/mm.h>
#include<linux/sched.h>
#include<linux/uaccess.h>
//#include<asm/io.h>

#define MEMDEV_MAJOR 260
#define MEMDEV_NR_DEVS 2
#define MEMDEV_SIZE 4096

struct mem_dev{
  char *data;
  unsigned long size;
};

static int mem_major = MEMDEV_MAJOR;
struct mem_dev *mem_devp; /*设备结构体指针*/
struct cdev cdev;

int mem_open(struct inode *inode, struct file *filp){
    struct mem_dev *dev;

    /*获取次设备号*/
    int num = MINOR(inode->i_rdev);

    if (num >= MEMDEV_NR_DEVS)
            return -ENODEV;
    dev = &mem_devp[num];

    /*将设备描述结构指针赋值给文件私有数据指针*/
    filp->private_data = dev;

    return 0;
}

int mem_release(struct inode *inode, struct file *filp){
  return 0;
}

static ssize_t mem_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos){
  unsigned long p = *ppos;
  unsigned int count = size;
  int ret = 0;
  struct mem_dev *dev = filp->private_data; /*获得设备结构体指针*/

  /*判断读位置是否有效*/
  if (p >= MEMDEV_SIZE)
    return 0;
  if (count > MEMDEV_SIZE - p)
    count = MEMDEV_SIZE - p;

  /*读数据到用户空间*/
  if (copy_to_user(buf, (void*)(dev->data + p), count))
    ret = - EFAULT;
  else{
    *ppos += count;
    ret = count;
    printk("read %d bytes(s) from %d\n", count, p);
  }
  return ret;
}

static ssize_t mem_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos){
  unsigned long p = *ppos;
  unsigned int count = size;
  int ret = 0;
  struct mem_dev *dev = filp->private_data; /*获得设备结构体指针*/

  /*分析和获取有效的写长度*/
  if (p >= MEMDEV_SIZE)
    return 0;
  if (count > MEMDEV_SIZE - p)
    count = MEMDEV_SIZE - p;

  /*从用户空间写入数据*/
  if (copy_from_user(dev->data + p, buf, count))
    ret = - EFAULT;
  else
  {
    *ppos += count;
    ret = count;

    printk(KERN_INFO "written %d bytes(s) from %d\n", count, p);
  }

  return ret;
}

/*文件操作结构体*/
static const struct file_operations mem_fops =
{
  .owner = THIS_MODULE,
  .read = mem_read,
  .write = mem_write,
  .open = mem_open,
  .release = mem_release,
};

static int init(void){
  int result;
  int i;

  dev_t devno = MKDEV(mem_major, 0);

  /* 静态申请设备号*/
  if (mem_major)
    result = register_chrdev_region(devno, 2, "memdev");
  else /* 动态分配设备号 */
  {
    result = alloc_chrdev_region(&devno, 0, 2, "memdev");
    mem_major = MAJOR(devno);
  }

  if (result < 0)
    return result;

  /*初始化cdev结构*/
  cdev_init(&cdev, &mem_fops);
  cdev.owner = THIS_MODULE;
  cdev.ops = &mem_fops;

  /* 注册字符设备 */
  cdev_add(&cdev, MKDEV(mem_major, 0), MEMDEV_NR_DEVS);

  /* 为设备描述结构分配内存*/
  mem_devp = kmalloc(MEMDEV_NR_DEVS * sizeof(struct mem_dev), GFP_KERNEL);
  if (!mem_devp) /*申请失败*/
  {
    result = - ENOMEM;
    goto fail_malloc;
  }
  memset(mem_devp, 0, sizeof(struct mem_dev));

  /*为设备分配内存*/
  for (i=0; i < MEMDEV_NR_DEVS; i++)
  {
        mem_devp[i].size = MEMDEV_SIZE;
        mem_devp[i].data = kmalloc(MEMDEV_SIZE, GFP_KERNEL);
        memset(mem_devp[i].data, 0, MEMDEV_SIZE);
  }

  return 0;

  fail_malloc:
  unregister_chrdev_region(devno, 1);

  return result;
}

static void exit(void){
  cdev_del(&cdev); /*注销设备*/
  kfree(mem_devp); /*释放设备结构体内存*/
  unregister_chrdev_region(MKDEV(mem_major, 0), 2); /*释放设备号*/
}

module_init(init);
module_exit(exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Cdevices");
MODULE_AUTHOR("JxT");
