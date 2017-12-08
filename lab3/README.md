# linux 设备驱动程序开发

## 设计目的

Linux 驱动程序占了内核代码的一半以上，开发设计驱动程序是 linux 内核编程的一项很重要的工作。通过本次实验，学习者应了解 linux 的设备管理机制及驱动程序的组织结构；掌握 linux 设备驱动程序的编写流程及加载方法，为从事具体的硬件设备驱动程序开发打下基础。

## 内容要求

* 编写一个字符设备驱动程序，要求实现对该字符设备的打开、读、写、I/O 控制和关闭 5 个基本操作。为了避免牵涉到汇编语言，这个字符设备并非一个真实的字符设备，而是用一段内存空间来模拟的。以模块方式加载该驱动程序。
* 编写一个应用程序，测试（1）中实现的驱动程序的正确性。
* 有兴趣的同学还可以编写一个块设备的驱动程序。
* 请根据自身情况，进一步阅读分析程序中用到的相关内核函数的源码实现。

## 注意事项

* 使用一段内存来模拟

* 涉及的数据结构：
    * struct cdev
    * struct char_device_struct
    * file_operations
    * file
    * inode
```C
struct cdev
{
    struct kobject kobj;               /*内嵌的内核对象，包括引用计数、名称、父指针等*/
    struct module *owner;              /*所属内核模块，一般设置为 THIS_MODULE */
    const struct file_operations *ops; /*设备操作集合 */
    struct list_head list;             /*设备的 inode 链表头 */
    dev_t dev;                         /*设备号 */
    unsigned int count;                /*分配的设备号数目 */
};

static struct char_device_struct
{
    struct char_device_struct *next; /* 指向散列链表中的下一个元素的指针*/
    unsigned int major;              /* 主设备号*/
    unsigned int baseminor;          /* 起始次设备号*/
    int minorct;                     /* 次设备号区间大小*/
    char name[64];                   /* 设备名*/
    struct file_operations *fops;    /* 未使用*/
    struct cdev *cdev;               /* 指向字符设备描述符的指针*/
} * chrdevs[CHRDEV_MAJOR_HASH_SIZE];

struct file_operations
{
    struct module *owner;                                                     /*拥有该结构的模块，一般为 THIS_MODULE*/
    ssize_t (*read)(struct file *, char __user *, size_t, loff_t *);          /*从设备中读取数据*/
    ssize_t (*write)(struct file *, const char __user *, size_t, loff_t *);   /*向设备中写数据*/
    int (*ioctl)(struct inode *, struct file *, unsigned int, unsigned long); /*执行设备的 I/O
控制命令*/
    int (*open)(struct inode *, struct file *);                               /*打开设备文件*/
    int (*release)(struct inode *, struct file *);                            /*关闭设备文件*/
    ......
};

struct file
{
    mode_t fmode;                 /*文件模式，如 FMODE_READ，FMODE_WRITE*/
    loff_t f_pos;                 /*当前读写指针*/
    struct file_operations *f_op; /*文件操作函数表指针*/
    void *private_data;           /*非常重要，用于存放转换后的设备描述结构指针*/
    ......
};

struct inode
{
    dev_t i_rdev;        /*设备号*/
    struct cdev *i_cdev; /*该设备的 cdev 结构*/
    ......
};
````

* 初始化流程:
    * 分配设备号 register_chrdev_region
    * 定义 cdev 结构并初始化 cdev_init
    * 注册 cdev 结构 cdev_add
* 实现字符设备驱动程序的操作函数
    * 实现 file_operations 结构中要用到的函数
    * 添加 file_operations 成员
* 注销设备
    * 撤销 cdev 结构
    * 释放设备号