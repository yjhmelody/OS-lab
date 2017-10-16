#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#define __NR_mysyscall 333 

int main() {
    int pid;
    int flag;
    int nicevalue;
    int prio;
    int nice;
    
    printf("please input the params: pid, flag, nicevalue, prio, nice\n");
    scanf("%d %d %d %d %d", &pid, &flag, &nicevalue, &prio, &nice);

    int ret = syscall(__NR_mysyscall, pid, flag, nicevalue, &prio, &nice);
    if (ret != 0){
        printf("syscall error! error code:%d\n", ret);
        return 0;
    }
    printf("pid:%d, flag:%d, nicevalue:%d, prio:%d, nice:%d\n", pid, flag, nicevalue, prio, nice);
    return 0;
}