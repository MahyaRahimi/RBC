#ifndef TCPNET_H
#define TCPNET_H
#include <thread>
#include <vector>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <mutex>
#include <condition_variable>

extern std::mutex m;                    //for exclusive access to memory
extern std::condition_variable cv;      //blocks the calling thread until notified to resume
extern int in_buffer_sign;                        /*a sign for transferring the input to inputbuffer (it is not possible
                                             to define friend classes to transfer in_buffer_sign or myvector)*/
extern std::vector<unsigned char> myvector;      //each element of myvector is a byte of data


class TCPNet
{
public:
    TCPNet();
    int myconnect();
    int listening();
    void transmitting();
private:
    std::thread listener;
    std::thread transmitter;
    int sockfd = 0,n = 0;
    char recvBuff[1024];
    char buffer[256];
    struct sockaddr_in serv_addr;
    int s; //size
    int listenfd = 0;
    int connfd = 0;
    char sendBuff[100];
    int myint;                 //input data
    unsigned char mychar;               //input data transformed to char
    std::vector <unsigned char> data_to_transmit;


};

#endif // TCPNET_H
