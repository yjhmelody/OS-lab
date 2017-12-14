#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#define MSG_LEN 80
#define MSG_END 1

pthread_t reciver_id;
pthread_t sender_id;
pthread_mutex_t mutex;
int msg_id;

typedef struct msg_data
{
    long type;
    char text[MSG_LEN];
} msg_data;


void check_error(int err)
{
    if (err != 0)
    {
        printf("error:%d\n", err);
        exit(err);
    }
}

void *reciver(void *args)
{
    msg_data data;
    long msg_type = 0;
    int err;
    for (;;)
    {
        err = msgrcv(msg_id, &data, MSG_LEN, msg_type, 0);
        check_error(err);

        // when meet "end"
        if (strcmp(data.text, "end") == 0)
        {
            // send "over"
            strcpy(data.text, "end");
            data.type = MSG_END;
            err = msgsnd(msg_id, &data, MSG_LEN, IPC_NOWAIT);
            check_error(err);
            break;
        }

        printf("%s\n", data.text);
    }

    // 正常退出
    return (void *)0;
}

void *sender(void *args)
{
    msg_data data;
    int len;
    int err;
    while (~scanf("%s", data.text))
    {
        len = strlen(data.text);
        if (strcmp(data.text, "exit") == 0)
        {
            // send "end"
            err = msgsnd(msg_id, &data, MSG_LEN, IPC_NOWAIT);
            check_error(err);

            // wait for response
            len = msgrcv(msg_id, &data, MSG_LEN, MSG_END, 0);
            if (len == -1)
            {
                printf("error:%d\n", -1);
                exit(-1);
            }

            printf("%s\n", data.text);
            err = msgctl(msg_id, IPC_RMID, 0);
            check_error(err);
            break;
        }

        err = msgsnd(msg_id, &data, MSG_LEN, IPC_NOWAIT);
        check_error(err);
    }
    return (void *)0;
}

int main()
{
    int err;

    pthread_mutex_init(&mutex, NULL);
    msg_id = msgget((key_t)1234, 0666 | IPC_CREAT);

    err = pthread_create(&reciver_id, NULL, reciver, NULL);
    check_error(err);
    err = pthread_create(&sender_id, NULL, sender, NULL);
    check_error(err);
    sleep(30);
    return 0;
}