#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

int main() {
  int fd;
  char buf[1024] = "pipeline2.c";

  mkfifo("pipefile", 0777);

  if ((fd = open("pipefile", O_RDWR)) == -1) {
    perror("open fifo error!");
    exit(0);
  }
  printf("go\n");
  write(fd, buf, strlen(buf) + 1);
  close(fd);
  exit(0);
}
