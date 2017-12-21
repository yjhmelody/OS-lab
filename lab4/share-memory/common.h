#ifndef COMMOM_H_
#define COMMOM_H_

#define check_error(err)                                                 \
    do                                                                   \
    {                                                                    \
        if (err < 0)                                                     \
        {                                                                \
            fprintf(stderr, "错误为第%d行,错误码为%d\n", __LINE__, err); \
            exit(1);                                                     \
        }                                                                \
    } while (0);

#define check_NULL(err)                                             \
    do                                                              \
    {                                                               \
        if (err == NULL)                                            \
        {                                                           \
            fprintf(stderr, "错误为第%d行,不能为NULL\n", __LINE__); \
            exit(1);                                                \
        }                                                           \
    } while (0);

#define name1 "sem_sender"
#define name2 "sem_reciver"
#define key 1234
#define SHM_SIZE 512

typedef struct
{
    char text[512];
} message;

#endif
