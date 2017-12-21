#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "common.h"


sem_t *sem_send;
sem_t *sem_recive;

int main()
{
    // init
    char msg[512];
    char msg2[512];
    int shmid = shmget(key, sizeof(message), 0666 | IPC_CREAT);
    check_error(shmid);

    void* shmp = shmat(shmid, NULL, 0);

    sem_send = sem_open(name1, O_CREAT, 0666, 1);
    sem_recive = sem_open(name2, O_CREAT, 0666, 0);
    printf("init end\n");
    // end init

    printf("请输入: ");
    scanf("%s", msg);

    // 发送信息
    sem_wait(sem_send);
    strcpy(((message*)shmp)->text, msg);
    sem_post(sem_recive);

    // 接受信息
    sem_wait(sem_send);
    strcpy(msg, ((message*)shmp)->text);
    printf("sender 接受到: %s\n", msg);

    sem_close(sem_send);
	sem_close(sem_recive);

    // 移除有名信号量
	sem_unlink(name1);
	sem_unlink(name2);

    printf("sender end\n");
    return 0;
}