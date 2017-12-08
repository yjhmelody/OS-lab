#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
    char str[80] = "hello world! hello linux!";
    char read_str[80];
    printf("str: %s\n", str);
    
    int fd = open("./dev", O_RDWR);
    if (fd < 0)
    {
        printf("open error\n");
        return 0;
    }
    printf("fd: %d\n", fd);

    int count = write(fd, str, strlen(str));
    if (count < 0)
    {
        printf("write error\n");
        return 0;
    }
    printf("write count: %d\n", count);
    
    int ret = lseek(fd , 2, SEEK_SET);
    if ( ret != 0){
        printf("lseek error\n");
    }

    count = read(fd, read_str, 10);
    if(count < 0 ){
        printf("read error\n");
        return 0;
    }
    printf("read str:%s\n", read_str);
    count = read(fd, read_str, 10);
    if(count < 0 ){
        printf("read error\n");
        return 0;
    }
    printf("read count: %d\n", count);
    printf("read str: %s\n", read_str);
    
    ret = lseek(fd , 10, SEEK_CUR);
    if ( ret != 0){
        printf("lseek error\n");
    }

    count = read(fd, read_str, 3);
    if(count < 0 ){
        printf("read error\n");
        return 0;
    }
    printf("read count: %d\n", count);
    printf("read str: %s\n", read_str);
    
    close(fd);
    printf("close\n");
    return 0;
}