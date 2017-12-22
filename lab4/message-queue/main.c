#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define N 4096

struct msg_data {
  long type;
  char text[N];
};

sem_t s1, s2, s3;

int id;
void *sender() {
  while (1) {
    sem_wait(&s1);

    printf("please input :");
    char s[111];
    scanf("%s", s);
    struct msg_data tmp;
    tmp.type = 1;
    if (strcmp(s, "exit") == 0) {
      memcpy(tmp.text, "end", sizeof("end"));
      msgsnd(id, &tmp, sizeof(tmp), 0); /// msgflg
      sem_post(&s2);

      sem_wait(&s1);
      msgrcv(id, &tmp, sizeof(tmp), 2, 0);
      if (strcmp(tmp.text, "over") == 0) {
        sem_post(&s2);
        break;
      }
    } else {
      memcpy(tmp.text, s, sizeof(s));
      int d = msgsnd(id, &tmp, sizeof(tmp), 0);
      if (d == 0)
        printf("send success\n");
      else
        printf("send failed \n");
    }

    sem_post(&s2);
  }
  printf("sender over\n");
  return NULL;
}

void *receiver() {
  while (1) {
    sem_wait(&s2);
    char s[111];

    struct msg_data tmp;
    printf("receicing...\n");
    long type = 1;
    int x = msgrcv(id, &tmp, sizeof(tmp), type, 0);
    printf("finish ...\n");

    printf("Receive :%s\n", tmp.text);
    if (strcmp(tmp.text, "end") == 0) {
      tmp.type = 2;
      memcpy(tmp.text, "over", sizeof("over"));
      msgsnd(id, &tmp, sizeof(tmp), 0);

      sem_post(&s1);
      break;
    }
    printf("%d\n", x);

    sem_post(&s1);
  }
  printf("receiver over\n");
  return NULL;
}

int main() {
  sem_init(&s1, 0, 1);
  sem_init(&s2, 0, 0);

  id = msgget(0, 0666);
  if (id < 0) {
    printf("msgqueue build error\n");
    return 0;
  }
  printf("ID=%d\n", id);
  pthread_t id1, id2;
  int t1 = pthread_create(&id1, NULL, sender, NULL);
  if (t1 < 0) {
    printf("t1 error\n");
    return 0;
  }
  int t2 = pthread_create(&id2, NULL, receiver, NULL);
  if (t2 < 0) {
    printf("t2 error\n");
    return 0;
  }

  pthread_join(id1, NULL);
  pthread_join(id2, NULL);
  printf("main over\n");
  return 0;
}

/**
mq
sender
receiver

linux neicundiaodu
**/
