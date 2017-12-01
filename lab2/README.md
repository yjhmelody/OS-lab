# linux 内核模块编程

## 设计目的

Linux 提供的模块机制能动态扩充 linux 功能而无需重新编译内核，已经广泛应用在 linux内核的许多功能的实现中。在本实验中将学习模块的基本概念、原理及实现技术，然后利用内核模块编程访问进程的基本信息，从而加深对进程概念的理解、对模块编程技术的掌握。

## 内容要求

* 设计一个模块，要求列出系统中所有内核线程的程序名、PID 号、进程状态及进程优先级。
* 设计一个带参数的模块，其参数为某个进程的 PID 号，该模块的功能是列出该进程的家族信息，包括父进程、兄弟进程和子进程的程序名、PID 号。
* 请根据自身情况，进一步阅读分析程序中用到的相关内核函数的源码实现。

## 注意事项

* 涉及到的命令：make, insmod, rmmod
* insmod 时，需要的是.ko文件
* 第二个模块需要在insmod时候按名传参


## 难点

task_struct里包含了四个优先级：

```C
struct task_struct {
                ……
                int prio, static_prio, normal_prio;
                unsigned int rt_priority;
                …… 
        };

// 在内核头文件include/linux/sched.h中定义了如下宏
#define    MAX_USER_RT_PRIO 100
#define    MAX_RT_PRIO MAX_USER_RT_PRIO
#define    MAX_PRIO (MAX_RT_PRIO + 40)
```

内核中规定进程的优先级范围为[0, MAX_PRIO-1]。其中实时任务的优先级范围是[0, MAX_RT_PRIO-1]，非实时任务的优先级范围是[MAX_RT_PRIO, MAX_PRIO-1]。

优先级值越小，意味着级别越高，任务先被内核调度。

那任务的优先级又是如何确定的呢？和task_struct中的成员是什么关系？

* prio指的是任务当前的动态优先级，其值影响任务的调度顺序。
* normal_prio指的是任务的常规优先级，该值基于static_prio和调度策略计算。
* static_prio指的是任务的静态优先级，在进程创建时分配，该值会影响分配给任务的时间片的长短和非实时任务动态优先级的计算。
* rt_priority指的是任务的实时优先级。若为0表示是非实时任务，[1, 99]表示实时任务，值越大，优先级越高。

* static_prio = MAX_RT_PRIO + 20 + nice（nice的缺省值是0，范围[20, 19]）
* rt_priority缺省值为0，表示非实时任务。[1，99]表示实时任务
* 对于实时任务，prio = normal_prio = static_prio
* 对于非实时任务 prio = normal_prio = MAX_RT_PRIO – 1 – rt_priority
* prio的值在使用实时互斥量时会暂时提升，释放后恢复成normal_prio