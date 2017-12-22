#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void read_from_pipe(int fd) {
  char buf[100];
  read(fd, buf, 100);
  printf("message is :%s\n", buf);
}

void write_to_pipe(int fd) {
  char *buf = "for the test!";
  write(fd, buf, strlen(buf));
}

int main() {
  pid_t pid;
  int fd[2];
  if (pipe(fd)) {
    printf("pipe failed!\n");
    exit(0);
  }
  pid = fork();
  if (pid == -1) {
    printf("frok failed\n");
    exit(0);
  } else if (pid == 0) {
    //        close(fd[1]);
    read_from_pipe(fd[0]);
    exit(0);
  } else {
    //        close(fd[0]);
    write_to_pipe(fd[1]);
    exit(0);
  }
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
