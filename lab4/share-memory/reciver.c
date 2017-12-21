#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/shm.h>
#include "common.h"


sem_t *sem_send;
sem_t *sem_recive;

int main()
{
    // init
    char msg[512];    
    int shmid = shmget(key, sizeof(message), IPC_CREAT | 0666);
    check_error(shmid);

    void *shmp = shmat(shmid, NULL, 0);

    sem_send = sem_open(name1, O_CREAT, 0666, 1);
    sem_recive = sem_open(name2, O_CREAT, 0666, 0);

    sem_wait(sem_recive);
    // reciver a msg
    strcpy(msg, ((message*)shmp)->text);
    printf("接受的消息: %s\n", msg);
    strcpy(((message*)shmp)->text, "over");
    sem_post(sem_send);

    sem_close(sem_send);
	sem_close(sem_recive);

    printf("receiver end\n");
    return 0;
}