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

/************************************************************************************************************************************************/
/*                            This function listens to TCP, connects to the antenna,                                                                 */
/*                            receives the frames and sends them to inputbuffer                                                                 */
/************************************************************************************************************************************************/



int listener::listening()
{

    {
        std::cout<<"here listening begins; listening_sign: "<<listening_sign<<std::endl;
        std::unique_lock<std::mutex> lk(m_lis_tcp);
        while(!listening_sign) cv_lis_tcp.wait(lk);
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
        std::cout<<"Here listening_sign in listener has changed; listening_sign: "<< listening_sign <<std::endl;
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

        listening_sign = 0;
    }
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    /* notify listener to unlock and wait for another input */
    cv_lis_tcp.notify_all();


    while(1)
    {
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
        std::cout<<"dakhele while"<<std::endl;
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* mutex between listener and inputbuffer*/
        /* lock to take the input: */
        {
            std::unique_lock<std::mutex> lk(m);
            /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
            std::cout<<"before read" <<std::endl;
            /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

            memset(buffer, 0, sizeof(buffer));
            n = read(connfd, buffer ,100);
            /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
            std::cout<<"after read" <<std::endl;
            /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

            if (n < 0)
            {
                printf("ERROR reading from socket\n");
                close(connfd);
            }
            else{
                printf("\nHere is the message: %s\n",buffer);
                /*memset(sendBuff, 0, sizeof(sendBuff));
                strcpy(sendBuff, "I got your message\n");
                n = writing(connfd, sendBuff, strlen(sendBuff));*/
                if (n < 0) printf("ERROR writing to socket");
                std::copy(&buffer[0], &buffer[100], back_inserter(myvector));
            }
            in_buffer_sign = 1;         //if in_buffer_sign is equal to 1, after notification inputbuffer will be unlocked.

        }
        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

        /* notify inputbuffer thread to finish waiting and start: */
        cv.notify_all();

        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
        std::cout << "new listened vector is: ";
        for (auto v : myvector)
                std::cout << std::hex << int(v) << " ";
        std::cout << std::endl;
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /*wait till inputbuffer thread saves myvector in myinputbuffer and change in_buffer_sign to 0: */
        {
            std::unique_lock<std::mutex> lk(m);
            while(in_buffer_sign) cv.wait(lk);
        }
        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
        std::cout<<"back to the listner" <<std::endl;

        /* send messages to the client via TCP: */
        /*memset(sendBuff, 0, sizeof(sendBuff));
        strcpy(sendBuff, "\nMessage from server in listener\n");
        writing(connfd, sendBuff, strlen(sendBuff));*/
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

        /* clear myvector to be able to fill in with new data: */
        myvector.clear();
    }
    close(connfd);
    close(listenfd);
    return 0;

}




