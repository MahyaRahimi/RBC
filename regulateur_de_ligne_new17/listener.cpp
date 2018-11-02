#include "listener.h"
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
#include <iostream>

/************************************************************************************************************************************************/
/*                            This function listens to TCP, connects to trains,                                                                 */
/*                            receives the frames and sends them to inputbuffer                                                                 */
/************************************************************************************************************************************************/

void listener::error_server(const char *msg)
{
perror(msg);
exit(1);
}
void listener::dostuff (int sock)
{
    int n;
    char buffer[256];
    std::cout<<"i am in dostuff\n";
    bzero(buffer,256);
    char sendBuff[1025];
    memset(sendBuff, '0', sizeof(sendBuff));
    strcpy(sendBuff, "Message from server");
    write(sock, sendBuff, strlen(sendBuff));
    while (1) {
        n = read(sock,buffer,255);
        if (n < 0) error_server("ERROR reading from socket");
        std::cout<<"do print::::\n";
        printf("Here is the message: %s\n",buffer);
        n = write(sock,"I got your message",18);
        if (n < 0) error_server("ERROR writing to socket");
    }

}

int main(void)
{


    return 0;//we never reach here
}

void listener::listen()
{
    int listenfd = 0,connfd = 0, pid;

    struct sockaddr_in serv_addr;

    char sendBuff[1025];
    int numrv;

    listenfd = socket(AF_INET, SOCK_STREAM, 0); //didn't close it!!!
    printf("socket retrieve success\n");

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));

    if(listen(listenfd, 10) == -1){
        printf("Failed to listen\n");
        return -1;
    }

    while(1)
    {
        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* lock to take the input: */
        {
            std::unique_lock<std::mutex> lk(m);
            connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL); // accept awaiting request
            if (connfd < 0)
                error_server("ERROR on accept");
            pid = fork();
            std::cout<<"forked"<<std::endl;
            if (pid < 0)
                error_server("ERROR on fork");
            if (pid == 0)
            {
                std::cout<<"pid==0"<<std::endl;
                /*close(listenfd);*/
                /*dostuff(connfd);*/

                /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
                /*               dostuff:::                 */
                int n;
                char buffer[256];
                std::cout<<"i am in dostuff\n";
                bzero(buffer,256);
                char sendBuff[1025];
                memset(sendBuff, '0', sizeof(sendBuff));
                strcpy(sendBuff, "Message from server");
                write(sock, sendBuff, strlen(sendBuff));
                while (1) {
                    n = read(sock, &myvector[0] ,255);
                    if (n < 0) error_server("ERROR reading from socket");
                    in_buffer_sign = 1;         //if in_buffer_sign is equal to 1, after notification inputbuffer will be unlocked.
                    std::cout << "myvector filled. in_buffer_sign is "<<in_buffer_sign<<std::endl;
                    std::cout<<"do print::::\n";
                    printf("Here is the message: %s\n",buffer);
                    n = write(sock,"I got your message",18);
                    if (n < 0) error_server("ERROR writing to socket");
                }
                /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

                exit(0);
            }
            else close(connfd);

        }

        //&&&&&&&&&&&&&lock bayad vasate while 1 e ghabli baste beshe o notify beshe. az inja be bad edit nashode:
        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

        /* notify inputbuffer thread to finish waiting and start: */
        cv.notify_all();

        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
        std::cout << "new listened vector is: ";
        for (auto v : myvector)
                std::cout << std::hex << int(v) << "\n";
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /*wait till inputbuffer thread saves myvector in myinputbuffer and change in_buffer_sign to 0: */
        {
            std::unique_lock<std::mutex> lk(m);
            while(in_buffer_sign) cv.wait(lk);
        }
        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

        /* clear myvector to be able to fill in with new data: */
        myvector.clear();






    }





    /* three inputs will be taken from user for test: */
    while(1)
    {
        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* lock to take the input: */
        {
            std::unique_lock<std::mutex> lk(m);
            std::cout << "Please enter an integer between 0 and 255 (enter 256 to end):\n";

            do {
              std::cin >> std::hex >> myint;
              if(myint<256){
                  mychar = myint;
                  myvector.push_back (mychar);
                  std::cout << "number is correct \n";
              }else{
                  in_buffer_sign = 1;         //if in_buffer_sign is equal to 1, after notification inputbuffer will be unlocked.
                  std::cout << "256 intered. in_buffer_sign is "<<in_buffer_sign<<std::endl;
              }
            } while (myint!=256);
        }
        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

        /* notify inputbuffer thread to finish waiting and start: */
        cv.notify_all();

        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
        std::cout << "new listened vector is: ";
        for (auto v : myvector)
                std::cout << std::hex << int(v) << "\n";
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /*wait till inputbuffer thread saves myvector in myinputbuffer and change in_buffer_sign to 0: */
        {
            std::unique_lock<std::mutex> lk(m);
            while(in_buffer_sign) cv.wait(lk);
        }
        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

        /* clear myvector to be able to fill in with new data: */
        myvector.clear();
    }

}
