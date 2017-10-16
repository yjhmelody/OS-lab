# linux 内核编译及添加系统调用

## 内容要求

（1）添加一个系统调用，实现对指定进程的 nice 值的修改或读取功能，并返回进程最

新的 nice 值及优先级 prio。建议调用原型为：

```C
int mysetnice(pid_t pid, int flag, int nicevalue, void __user * prio, void __user * nice);
```

参数含义：

pid：进程 ID。

flag：若值为 0，表示读取 nice 值；若值为 1，表示修改 nice 值。

Prio、nice：进程当前优先级及 nice 值。

返回值：系统调用成功时返回 0，失败时返回错误码 EFAULT。

（2）写一个简单的应用程序测试（1）中添加的系统调用。

（3）若程序中调用了 linux 的内核函数，要求深入阅读相关函数源码。

## 源码位置

syscall_64.tbl 
./arch/x86/entry/syscalls/syscall_64.tbl

sys.c
./kernel/sys.c

syscalls.h
/include/linux/syscalls.h
