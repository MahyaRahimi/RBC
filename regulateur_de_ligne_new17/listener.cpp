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
#include <iterator>
#include <algorithm>
#include <mutex>
#include <condition_variable>
/************************************************************************************************************************************************/
/*                            This function listens to TCP, connects to trains,                                                                 */
/*                            receives the frames and sends them to inputbuffer                                                                 */
/************************************************************************************************************************************************/

void listener::error_server(const char *msg)
{
perror(msg);
exit(1);
}
/*void listener::dostuff (int sock)
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

}*/


int listener::listening()
{
    int listenfd = 0,connfd = 0, pid;

    struct sockaddr_in serv_addr;

    char sendBuff[1025];

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
            char buffer[100];
            std::cout<<"i am in dostuff\n";
            //bzero(&myvector[0],100);
            bzero(buffer,100);
            /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
            std::cout<<"do print::::\nbuffer is:";
            for (int i=0; i<100; i++)
                std::cout << int(buffer[i]) << " ";
            std::cout << "myvector is: ";
            int k=0;
            for (auto v : myvector)
            {
                    std::cout << k << ":" << std::hex << int(v) << "\n";
                    k++;
            }
            /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

            char sendBuff[100];
            memset(sendBuff, '0', sizeof(sendBuff));
            strcpy(sendBuff, "Message from server");
            write(connfd, sendBuff, strlen(sendBuff));
            while (1) {
                /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
                /* lock to take the input: */
                {
                    std::lock_guard<std::mutex> lk(m10);
                    //n = read(connfd, &myvector[0] ,100);
                    n = read(connfd, buffer ,100);
                    std::copy(&buffer[0], &buffer[100], back_inserter(myvector));
                    if (n < 0) error_server("ERROR reading from socket");
                    in_buffer_sign = 1;         //if in_buffer_sign is equal to 1, after notification inputbuffer will be unlocked.
                    std::cout << "myvector filled. in_buffer_sign is "<<in_buffer_sign<<std::endl;
                    std::cout<<"do print::::\nbuffer is:";
                    for (int i=0; i<100; i++)
                        std::cout << buffer[i] << " ";
                    printf("Here is the message: %s\n",&myvector[0]);

                    /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
                    std::cout << "new listened vector is: ";
                    int k=0;
                    for (auto v : myvector)
                    {
                            std::cout << k << ":" << std::hex << int(v) << "\n";
                            k++;
                    }
                    /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

                    n = write(connfd,"I got your message",18);
                    if (n < 0) error_server("ERROR writing to socket");
                }
                /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
                /* notify inputbuffer thread to finish waiting and start: */
                std::cout<<"time to notify"<<std::endl;
                //lk.unlock();
                cv10.notify_all();

                /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
                std::cout<<"here in_buffer_sign in listener has changed; in_buffer_sign: "<<in_buffer_sign<<std::endl;
                /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

                /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
                /*wait till inputbuffer thread saves myvector in myinputbuffer and change in_buffer_sign to 0: */
                {
                    std::unique_lock<std::mutex> lk(m10);
                    while(in_buffer_sign) cv10.wait(lk);
                }
                /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
                std::cout<<"back to the listner" <<std::endl;
                /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
                /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

                /* clear myvector to be able to fill in with new data: */
                myvector.clear();

            }
            /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
            exit(0);
        }
        else close(connfd);

    }

}
