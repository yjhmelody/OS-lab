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

#define check_error(err)                                                 \
    do                                                                   \
    {                                                                    \
        if (err != 0)                                                    \
        {                                                                \
            fprintf(stderr, "错误为第%d行,错误码为%d\n", __LINE__, err); \
            exit(err);                                                   \
        }                                                                \
    } while (0);

void *sender()
{
    struct message msg;
    msg.message_type = 1;
    int flag = -1;
    int err;

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
        err = msgsnd(msgid, &msg, sizeof(struct message), 0);
        check_error(err);
        sem_post(&send);
    }
    sem_wait(&point);

    err = msgrcv(msgid, &msg, sizeof(struct message), 0, 0);
    check_error(err);

    //  if (msgrcv(msgid, &msg, sizeof(struct message), 0, 0) == -1)
    // {
    // fprintf(stderr, "sender receive failed\n");
    // exit(1);
    // };

    sem_post(&send);
    err = strncmp(msg.msg, "over", 4);
    check_error(err);

    printf("final res:%s\n", msg.msg);
    err = msgctl(msgid, IPC_RMID, 0);
    check_error(err);
    return (void *)0;
}
void *receive()
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
    return (void *)0;
}
int main()
{
    pthread_t id1, id2;
    int err;

    sem_init(&send, 1, 0);
    sem_init(&point, 1, 0);
    sem_init(&rec, 1, 1);
    msgid = msgget((key_t)KEY, IPC_CREAT | 0666);

    err = pthread_create(&id1, NULL, sender, NULL);
    check_error(err);
    err = pthread_create(&id2, NULL, receive, NULL);
    check_error(err);

    pthread_join(id1, NULL);
    pthread_join(id2, NULL);

    return 0;
}