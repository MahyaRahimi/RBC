#ifndef TCP_H
#define TCP_H
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <mutex>
#include <condition_variable>

extern std::mutex m_lis_tcp;                    //for exclusive access to memory_between tcp and listener
extern std::condition_variable cv_lis_tcp;      //blocks the calling thread until notified to resume
extern int listening_sign;                      //a sign for notifying tcp that listener has restarted


class TCP
{
public:
    TCP();
    int connecting();
    int listenfd = 0;
    int connfd = 0;
    struct sockaddr_in serv_addr;
    char buffer[100];
    char sendBuff[100];
    int n;
    int myint;                 //input data
    unsigned char mychar;               //input data transformed to char
};

#endif // TCP_H
