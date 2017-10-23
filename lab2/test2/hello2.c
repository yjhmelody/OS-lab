#include"linux/kernel.h"

void hello2(void){
    printk(KERN_ALERT"hello2\n");
}

EXPORT_SYMBOL_GPL(hello2);
