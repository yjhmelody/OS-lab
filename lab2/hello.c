#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>


/**
 *  第 1、2 行是模块编程的必需头文件。module.h 包含了大量加载模块所需要的函数和符
    号的定义；init.h 包含了模块初始化和清理函数的定义。如果模块在加载时允许用户传递参
    数，模块还应该包含 moduleparam.h 头文件。
 * /


/**
 *  如果一个模块需要向其他模块导出符号，可使用下面的宏：
    EXPORT_SYMBOL(symbol_name)；
    EXPORT_SYMOBL_GPL(symbol_name)；
    这两个宏均用于将给定的符号导出到模块外部。_GPL 版本使得要导出的符号只能被 GPL
    许可证下的模块使用。符号必须在模块文件的全局部分导出，不能在模块中的某个函数中导出。
*/

/**
  * 内核模块程序中没有 main 函数，每个模块必须定义两个函数：一个函数用来初始化，
    主要完成模块注册和申请资源，该函数返回 0，表示初始化成功，其他值表示
    失败；另一个函数用来退出，主要完成注销和释放资源。Linux 调用宏
    module_init 和 module_exit 来注册这两个函数，module_init宏标记的函数在加载模块时调用，
    module_exit 宏标记的函数在卸载模块时调用。需要注意的是，
    初始化与清理函数必须在宏 module_init 和 module_exit 使用前定义，否则会出现编译错误。
*/


// 一般情况下，初始化函数应当申明为 static，以便它们不会在特定文件之外可见。如果
// 该函数只是在初始化使用一次，可在声明语句中加__init 标识，则模块在加载后会丢弃这个
// 初始化函数，释放其内存空间
static int hello_init(){
    printk(KERN_ALERT"hello world\n");
}

// 清理函数没有返回值，因此被声明为 void。声明语句中的__exit 的含义与初始化函数中
// 的__init 类似
static void hello_exit(){
    printk(KERN_ALERT"good bye\n");
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");

