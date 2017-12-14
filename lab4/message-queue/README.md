# 利用 linux 的消息队列通信机制实现两个线程间的通信

编写程序创建两个线程：sender 线程和 receive 线程，其中 sender 线程运行函数 sender()，它创建一个消息队列，然后，循环等待用户通过终端输入一串字符，将这串字符通过消息队列发送给 receiver 线程，直到用户输入“exit”为止；最后，它向 receiver 线程发送消息“end”，并且等待 receiver 的应答，等到应答消息后，将接收到的应答信息显示在终端屏幕上，删除相关消息队列，结束程序的运行。Receiver 线程运行 receive()，它通过消息队列接收来自sender 的消息，将消息显示在终端屏幕上，直至收到内容为“end”的消息为止，此时，它向 sender 发送一个应答消息“over”，结束程序的运行。使用无名信号量实现两个线程之间的同步与互斥。


所需要的头文件
```c
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
```

