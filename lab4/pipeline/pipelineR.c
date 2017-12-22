#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int fd;
  char buf[1024];

  fd = open("pipefile", O_RDWR);
  printf("go\n");
  read(fd, buf, 1024);
  printf("The message is :%s\n", buf);
  close(fd);
  return 0;
}
