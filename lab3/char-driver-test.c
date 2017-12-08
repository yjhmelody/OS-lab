#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main()
{
    char str[80];
    scanf("%s", str);
    int fd = open("/dev/yjh_char_dev");
    printf("%s\n", str);
    int count = write(fd, str, strlen(str));
    printf("fd: %d\n", fd);
    printf("count: %d\n", count);
}