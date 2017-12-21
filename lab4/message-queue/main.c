#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <semaphore.h>

#define MSG_LEN 512
#define MSG_END 1

pthread_t reciver_id;
pthread_t sender_id;

sem_t sem_empty;
sem_t sem_full;
sem_t sem_end;

int msg_id;

typedef struct msg_data
{
    long type;
    char text[MSG_LEN];
} msg_data;

#define MSG_DATA_SIZE (sizeof(msg_data))

#define check_error(err)                                                 \
    do                                                                   \
    {                                                                    \
        if (err < 0)                                                     \
        {                                                                \
            fprintf(stderr, "错误为第%d行,错误码为%d\n", __LINE__, err); \
            exit(err);                                                   \
        }                                                                \
    } while (0);

void *sender(void *args)
{
    printf("sender\n");
    msg_data data;
    data.type = 0;

    int len;
    int err;
    char str[MSG_LEN];

    for (;;)
    {
        sem_wait(&sem_empty);
        printf("sender: ");
        scanf("%s", str);

        if (strcmp(str, "exit") == 0)
        {
            printf("exit\n");
            strcpy(data.text, "end");
            err = msgsnd(msg_id, &data, MSG_DATA_SIZE, 0);
            check_error(err);

            sem_post(&sem_full);

            sem_wait(&sem_empty);
            msgrcv(msg_id, &data, MSG_DATA_SIZE, MSG_END, 0);
            if (strcmp(data.text, "over") == 0)
            {
                sem_post(&sem_full);
                break;
            }
        }
        else
        {
            strcpy(data.text, str);
            printf("your words: %s\n", data.text);
            err = msgsnd(msg_id, &data, MSG_DATA_SIZE, 0);
            check_error(err);           
        }
         sem_post(&sem_full);
    }

    err = msgctl(msg_id, IPC_RMID, 0);
    // check_error(err);

    return (void *)0;
}

void *reciver(void *args)
{
    printf("reciver\n");

    msg_data data;
    int err;
    int len;
    data.type = 0;

    for (;;)
    {
        // 不能交换
        sem_wait(&sem_full);

        len = msgrcv(msg_id, &data, MSG_DATA_SIZE, 0, 0);
        if (strcmp(data.text, "end") == 0)
        {
            printf("end\n");
            data.type = MSG_END;
            strcpy(data.text, "over");

            sem_post(&sem_empty);
            break;
        }
        printf("reciver: %s\n", data.text);
        sem_post(&sem_empty);
    }

    // 通知结束
    // sem_post(&sem_end);

    err = msgsnd(msg_id, &data, MSG_DATA_SIZE, 0);
    // check_error(err);

    return (void *)0;
}

int main()
{
    int err;
    sem_init(&sem_empty, 0, 1);
    sem_init(&sem_full, 0, 0);
    sem_init(&sem_end, 0, 0);

    msg_id = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

    err = pthread_create(&reciver_id, NULL, reciver, NULL);
    check_error(err);
    err = pthread_create(&sender_id, NULL, sender, NULL);
    check_error(err);

    // 函数pthread_join用来等待一个线程的结束,线程间同步的操作
    pthread_join(reciver_id, NULL);
    pthread_join(sender_id, NULL);
    return 0;
}