#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#define __NR_mysyscall 333

int main()
{
    int pid = 0;
    int flag = 0;
    int nicevalue = 0;
    int prio = 0;
    int nice = 0;
    int ret;

    printf("please input the params: pid, flag, nicevalue\n");
    while(~scanf("%d %d %d", &pid, &flag, &nicevalue))
    {
        ret = syscall(__NR_mysyscall, pid, flag, nicevalue, &prio, &nice);
        if (ret < 0)
        {
            printf("syscall error! error code:%d\n", ret);
            return 0;
        }
        printf("pid:%d, flag:%d, nicevalue:%d, prio:%d, nice:%d\n", pid, flag, nicevalue, prio, nice);
    }
    return 0;
}