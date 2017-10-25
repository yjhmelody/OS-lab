#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init_task.h>

MODULE_LICENSE("GPL");

static int hello_init(void)
{
    struct task_struct *task;

    for (task = &init_task; (task = next_task(task)) != &init_task;)
    {
        // kernel thread
        if (task->mm == NULL)
        {
            printk(KERN_ALERT "名称:%s\n进程号:%d\n状态:%ld\n优先级:%d\n",
            task->comm, task->pid, task->state, task->prio);
        }
    }
    return 0;
}

static void hello_exit(void)
{
    printk(KERN_ALERT "good bye thread-iteration\n");
}

module_init(hello_init);
module_exit(hello_exit);