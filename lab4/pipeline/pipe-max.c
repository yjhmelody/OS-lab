#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/msg.h>
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

#define size 1024
int main()
{
    pid_t pid;
    int fd[2];
    int err;
    char buf[size];
    int count = 0;
    int ret = 1;

    memset(buf, 'a', sizeof(buf));

    err = pipe(fd);
    check_error(err);

    while (ret != -1)
    {
        ret = write(fd[1], buf, sizeof(buf));
        count++;
        printf("size:%dkb\n", count);
    }
    return 0;
}
