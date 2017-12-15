#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init_task.h>
#include <linux/moduleparam.h>
#include <linux/pid.h>
#include <linux/list.h>

MODULE_LICENSE("GPL");

static int pid;
module_param(pid, int, 0755);

static int hello_init(void)
{
    struct task_struct *task = pid_task(find_vpid(pid), PIDTYPE_PID);
    if(task == NULL){
        printk(KERN_ALERT "bad pid\n");
        return -EFAULT;
    }
    struct task_struct *parent = task->parent;
    struct task_struct *p = NULL;

    struct list_head *sibling = NULL;
    struct list_head *child = NULL;

    printk(KERN_ALERT "父进程:%s,pid:%d\n", parent->comm, parent->pid);
    printk(KERN_ALERT "该进程:%s,pid:%d\n", task->comm, task->pid);

    list_for_each(sibling, &task->sibling)
    {
        p = list_entry(sibling, struct task_struct, sibling);
        if (p->pid != pid)
        {
            printk(KERN_ALERT "兄弟进程:%s,pid:%d\n", p->comm, p->pid);
        }
    }

    list_for_each(child, &task->children)
    {
        p = list_entry(child, struct task_struct, sibling);
        printk(KERN_ALERT "子进程:%s,pid:%d\n", p->comm, p->pid);
    }

    return 0;
}

static void hello_exit(void)
{
    printk(KERN_ALERT "good bye process-family-info\n");
}

module_init(hello_init);
module_exit(hello_exit);