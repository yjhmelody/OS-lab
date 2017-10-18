#include<linux/moduleparam.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
// 模块要带参数，则头文件必须包括：#include <linux/moduleparam.h>
MODULE_LICENSE("GPL");

// from hello.c
void hello2(void);

static int  __init hello_init(void){
    printk(KERN_ALERT"hello\n");
    return 0;
}

static void __exit hello_exit(void){
    printk(KERN_ALERT"exit\n");
}

module_init(hello_init);
module_exit(hello_exit);

