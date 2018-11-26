#ifndef LISTENER_H
#define LISTENER_H
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

extern std::mutex m;                    //for exclusive access to memory
extern std::condition_variable cv;      //blocks the calling thread until notified to resume
extern int in_buffer_sign;                        /*a sign for transferring the input to inputbuffer (it is not possible
                                             to define friend classes to transfer in_buffer_sign or myvector)*/
extern std::vector<unsigned char> myvector;      //each element of myvector is a byte of data

class listener
{
public:
    friend class transmitter;
    listener(){}
    int listening();
private:
    int listenfd = 0;
    int connfd = 0;
    struct sockaddr_in serv_addr;
    char buffer[100];
    char sendBuff[100];
    int n;
    int myint;                 //input data
    unsigned char mychar;               //input data transformed to char
};

#endif // LISTENER_H
