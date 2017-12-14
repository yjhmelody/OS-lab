#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/msg.h>
#include <pthread.h>
#include <semaphore.h>

#define KEY 2333
#define LENOFMSG 100

int msgid = 0;
sem_t send, rec, point;

struct message
{
    long int message_type;
    char msg[LENOFMSG];
};

//初始化信号量
int init()
{
    sem_init(&send, 1, 0);
    sem_init(&point, 1, 0);
    sem_init(&rec, 1, 1);
    msgid = msgget((key_t)KEY, IPC_CREAT | 0666);
    return msgid;
}

void sender()
{
    struct message msg;
    msg.message_type = 1;
    int flag = -1;
    while (flag != 1)
    {
        sem_wait(&rec);
        printf("Enter your msg:");
        scanf("%s", msg.msg);
        if (strcmp(msg.msg, "exit") == 0)
        {
            strcpy(msg.msg, "end");
            flag = 1;
        }
        if (strcmp(msg.msg, "end") == 0)
        {
            flag = 1;
        }
        if (msgsnd(msgid, &msg, sizeof(struct message), 0) == -1)
        {
            fprintf(stderr, "msgsnd failed\n");
            exit(1);
        };
        sem_post(&send);
    }
    sem_wait(&point);
    if (msgrcv(msgid, &msg, sizeof(struct message), 0, 0) == -1)
    {
        fprintf(stderr, "sender receive failed\n");
        exit(1);
    };
    sem_post(&send);
    if (strncmp(msg.msg, "over", 4) != 0)
    {
        printf("can't get received!\n");
        exit(1);
    }
    printf("final res:%s\n", msg.msg);
    if (msgctl(msgid, IPC_RMID, 0) == -1)
    {
        printf("delete sender msgqueue failed\n");
        exit(1);
    }
    return;
}

void receive()
{
    struct message msg;
    int flag = -1;
    while (flag != 1)
    {
        sem_wait(&send);
        if (msgrcv(msgid, &msg, sizeof(struct message), 1, 0) == -1)
        {
            fprintf(stderr, "msgrcv failed\n");
            exit(1);
        };
        printf("receive msg:%s\n", msg.msg);
        sem_post(&rec);
        if (strcmp(msg.msg, "end") == 0)
        {
            strcpy(msg.msg, "over");
            if (msgsnd(msgid, &msg, sizeof(struct message), 0) == -1)
            {
                fprintf(stderr, "receive--receive failed\n");
                exit(1);
            }
            flag = 1;
            sem_post(&point);
        }
    }
    sem_wait(&send);
    return; //正常退出，使用exit将使主线程也直接退出，不执行之后的代码
}

int main()
{
    pthread_t id1, id2;
    int initres = init();
    if (initres == -1)
    {
        printf("create receive msgqueue failed\n");
        exit(1);
    }
    int res = pthread_create(&id1, NULL, (void *)sender, NULL);
    if (res != 0)
    {
        printf("Create pthread sender error!\n");
        exit(1);
    }
    int res2 = pthread_create(&id2, NULL, (void *)receive, NULL);
    if (res2 != 0)
    {
        printf("Create pthread receive error!\n");
        exit(1);
    }
    pthread_join(id1, NULL);
    pthread_join(id2, NULL);
    printf("exit program!\n");
    return 0;
}