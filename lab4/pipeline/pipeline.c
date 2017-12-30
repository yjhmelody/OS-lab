#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<semaphore.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include <fcntl.h>

#define check_error(err)                                                 \
    do                                                                   \
    {                                                                    \
        if (err < 0)                                                     \
        {                                                                \
            fprintf(stderr, "错误为第%d行,错误码为%d\n", __LINE__, err); \
            exit(1);                                                     \
        }                                                                \
    } while (0);

void read_from_pipe(int fd){
    char buf[50];
    int n = read(fd,buf,sizeof(buf));
    buf[n] = '\0';
    printf("message is :\n%s\n", buf);
    printf("实际读取到的字节数: %d\n", n);
}

void write_to_pipe1(int fd){
    char *buf="test 1!\n";
    printf("LOOK1\n");
    int n = write(fd,buf,strlen(buf));
    printf("实际写入的字节数: %d\n", n);
}

void write_to_pipe2(int fd){
    char *buf="test 2!\n";
    printf("LOOK2\n");
    int n = write(fd,buf,strlen(buf));
    printf("实际写入的字节数: %d\n", n);
}

void write_to_pipe3(int fd){
    char *buf="test 3!\n";
    printf("LOOK3\n");
    int n = write(fd,buf,strlen(buf));
    printf("实际写入的字节数: %d\n", n);

}

#define name1 "mutex"
#define name2 "count"

int main(){
    pid_t pid1, pid2, pid3;
    int fd[2];
    int err;

    sem_t *mutex = sem_open(name1, O_CREAT, 0666, 1);
    sem_t *count = sem_open(name2, O_CREAT, 0666, 0);
    // printf("%d %d %d\n",cnt1->__align,cnt2->__align,cnt3->__align);
    // printf("%d\n",lock->__align);

    err = pipe(fd);
    check_error(err);
    
    // child 1
    pid1=fork();
    check_error(pid1);
    if(pid1 == 0){
        sem_wait(mutex);
        write_to_pipe1(fd[1]);
        sem_post(mutex);
        // sem_post(count);
        return 0;
    }

    // child 2
    pid2 = fork();
    check_error(pid2);
    if(pid2 == 0){
        sem_wait(mutex);
        write_to_pipe2(fd[1]);
        sem_post(mutex);
        // sem_post(count);
        return 0;
    }

    // child 3
    pid3 = fork();
    check_error(pid3);
    if(pid3 == 0){
        sem_wait(mutex);
        write_to_pipe3(fd[1]);
        sem_post(mutex);
        // sem_post(count);
        return 0;
    }
    wait(pid1);
    wait(pid2);
    wait(pid3);

    // sem_wait(count);
    // sem_wait(count);
    // sem_wait(count);
    read_from_pipe(fd[0]);

    sem_unlink(name1);
    sem_unlink(name2);
    return 0;
}

/**
SYSCALL_DEFINE1(pipe, int __user *, fildes){
     return sys_pipe2(fildes, 0);
}

SYSCALL_DEFINE2(pipe2, int __user *, fildes, int, flags){
     struct file *files[2];
     int fd[2];
     int error;
     error = __do_pipe_flags(fd, files, flags);
     if (!error) {
          // 一切准备就绪后 把刚才和管道关联的2个fd拷贝到用户空间
          if (unlikely(copy_to_user(fildes, fd, sizeof(fd)))) {
               fput(files[0]);
               fput(files[1]);
               put_unused_fd(fd[0]);
               put_unused_fd(fd[1]);
               error = -EFAULT;
          } else {
               // 把fd和file的映射关系更新到该进程的文件描述表中fdtable
               fd_install(fd[0], files[0]);
               fd_install(fd[1], files[1]);
          }
     }
     return error;
}

**/
