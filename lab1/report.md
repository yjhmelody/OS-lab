# linux 内核编译及添加系统调用

## 项目设计方案

### 总体设计思路

添加一个系统调用，实现对指定进程的 nice 值的修改或读取功能，并返回进程最新的 nice 值及优先级 prio：

* 严格地检查异常，遇到不合法的参数，立即返回错误码。调用其他例程时，不要忘记检查返回值。

* 因为总是需要返回最新的prio和nice值，所以flag=1和flag=0的差别在于是否需要使用nicevalue来修改。

* 返回值时，因为是从内核空间取数据，应该使用相应的内核函数来copy数据。

* 因为是根据pid来读写，所以需要遍历进程树。

### 接口设计

严格按照实验指导手册给定的接口设计

```C
int mysetnice(pid_t pid, int flag, int nicevalue, void __user * prio, void __user * nice);
```

### 主要函数程序设计思路与程序设计流程图

![程序流程图](../asset/lab1.png)

### 设计方案创新性分析

* 通过传入2个指针来达到返回3个值的目的，其中2个需要的数据，1个状态码。

## 项目实现过程

### 遇到的问题

* 首先遇到的问题时如何遍历进程，然后网上查资料找到了相应的内核函数。
* 实现后发现部分不合法参数没有处理，比如传入空指针，flag不为0或1。
* 如何把内核态数据拷贝到用户态下，需要了解`copy_to_user`该内核函数的功能。

### 分析程序运行结果

* 系统调用能正常运行，flag=1时，nice=nicevalue，并且prio总是比nice值大20。
* 传入不合法nicevalue时，不会修改nice，符合预期。

### 分析程序实现中的创新性

* 代码不冗余，因为读和写都需要返回最新的值，可以把部分代码段合并减少系统调用各种资源开销。
* 优先检查异常和参数的合法性，可以减少不必要的内存和CPU开销。
* 遍历过程中找到以后做相应操作即可，然后返回，无需break等跳转操作。

## 进一步思考

### 分析程序性能

同创新性所言，该实验的主要是在性能上创新

* 代码不冗余，因为读和写都需要返回最新的值，可以把部分代码段合并减少系统调用各种资源开销。
* 优先检查异常和参数的合法性，可以减少不必要的内存和CPU开销。
* 遍历过程中找到以后做相应操作即可，然后返回，无需break等跳转操作。

### 考虑的改进思路

修改系统调用的参数，可以实现更加通用的系统调用，比如加上options，根据options获取或修改进程对应数据，而不只是prio和nice。

### 你认为教师对本实验的设计是否合理？难易程度如何？有什么改进建议？

难度适中，不过许多细节跟操作系统原理本身没有太多的关联，不能反映操作系统的知识掌握情况。
这个实验可能更多难点在C语言上。而里面的linux知识，相比于后面的实验，资料更难查找。
给linux添加系统调用本身只需要了解其项目的各种开发流程即可，就跟github上面的许多开源项目一样，
linux也是在github开源，做相应的开发都需要了解其开发流程。所以实验1本身更偏向于软件工程的实践，
即加入到一个大型项目的所需要的一些基本素养的锻炼。

## 参考文献

* [Linux内核设计与实现(原书第3版)](https://book.douban.com/subject/6097773/)
* https://github.com/jijiwuming/HDU-OS-Experiment
* https://www.cnblogs.com/wangzahngjun/p/4992045.html

## 程序完整代码

```C
SYSCALL_DEFINE5(mysetnice, pid_t, pid, int, flag, int, nicevalue, void __user *, prio, void __user *, nice)
{
	// priority function's return value
	if(pid < 0 || (flag != 1 && flag != 0)){
		return -EFAULT;
	}

	if(nice == NULL || prio == NULL){
		return -EFAULT;
	}

	struct task_struct *p;
	int temp;
	// finder begins at init_task
	for(p = &init_task;(p = next_task(p)) != &init_task;){
		// when find the pid
		if(p->pid == pid){
			// set
			if(flag == 1){
				set_user_nice(p, nicevalue);
			}
			// get
			// need to transform type
			
			temp = task_nice(p);
			if(copy_to_user(nice, &temp, sizeof(int))){
				return -EFAULT;
			}
			temp = task_prio(p);
			if(copy_to_user(prio, &temp, sizeof(int))){
				return -EFAULT;
			}
			// *(int*)nice = (int)task_nice(p);
			// *(int*)prio = (int)task_prio(p);
			return 0;
		}
	}
	return -EFAULT;
}
```