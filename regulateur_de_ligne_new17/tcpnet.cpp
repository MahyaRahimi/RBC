#include "tcpnet.h"

TCPNet::TCPNet()
{

}



int TCPNet::myconnect()
{
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("socket retrieve success\n");

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, 0, sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(4000);

    if(bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
        std::cout << "Error in TCP socket bind!"<< std::endl;

    if(listen(listenfd, 10) == -1){
        printf("Failed to listen\n");
        return -1;
    }
    connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL); // accept awaiting request

    listener= std::thread(&TCPNet::listening, this);//function listening
    transmitter = std::thread(&TCPNet::transmitting, this); //function transmitting
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
    listener.join();
    transmitter.join();

    return 0;
}

int TCPNet::listening()
{

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
            n = read(connfd, buffer ,sizeof(buffer)-1);
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
                std::copy(&buffer[0], &buffer[100], back_inserter(myvector));  //copy buffer to myvector
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


void TCPNet::transmitting()
{
    while(1)
    {
        recvBuff[n] = 0;
        if(fputs(recvBuff, stdout) == EOF)
        {
            printf("\n Error : Fputs error");
        }
        //printf("\n");
        /*write:::*/
        printf("Please enter the message to server: ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);/* it reads from keyboard of the client */
        n = write(connfd,buffer,strlen(buffer));/* writes the entered message on the socket */
        if (n < 0)
            printf("ERROR writing to socket");
    }

}

//#include "transmitter.h"
//#include <vector>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <errno.h>
//#include <string.h>
//#include <sys/types.h>

///************************************************************************************************************************************************/
///********************************This function ideally receives the frames from the output buffer and transmits them to trains *************************/
///*******************Since we don't have correct XBee transmitter funtion, we simulate frame transmition in another way : ************************/
///************************************************************************************************************************************************/

//void transmitter::transmit()
//{
//    while(1){
//        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
//        /* locks the thread and waits till count1 becomes more than one in output buffer thread. it means that there exists something to transmit: */
//        std::unique_lock<std::mutex> lk4(m4);
//        std::cout << "now transmitter waits for count1. count1 in transmitter= "<<count1<<std::endl;
//        while(!count1) cv4.wait(lk4);
//        m4.unlock();
//        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

//        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
//        std::cout<<"we are in transmitter after wait; count1: "<< count1 <<std::endl;
//        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

//        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
//        /****************** start transmission of outnputs to trains: ************************************************************************************************************************/
//        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

//        /* retreive data from the vector myoutputbuffer: */
//        data_to_transmit = myoutputbuffer.front();

//        /* to delete the retreived data from the myoutputbuffer: */
//        myoutputbuffer.erase(myoutputbuffer.begin());

//        /* send messages to the client via TCP: */
//        memset(sendBuff, 0, sizeof(sendBuff));
//        strcpy(sendBuff, "Message from server in transmitter\n");
//        writing(connfd, sendBuff, strlen(sendBuff));

//        memset(sendBuff, 0, sizeof(sendBuff));
//        std::copy(data_to_transmit.begin(), data_to_transmit.end(), sendBuff);
//        writing(connfd, sendBuff, strlen(sendBuff));

//        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
//        /* print the message to be transmitted; instead of sending by XBEE artificially we print the message: */
//        std::cout << "\n\n\n******\nHere is the message to the train: \n******\n\n\n";
//        //int_data_to_transmit = data_to_transmit;
//        for ( int i = 0; i<data_to_transmit.size(); i++ ){
//            //int_data_to_transmit[i] = data_to_transmit[i];
//            std::cout << std::hex << int(data_to_transmit[i]) << "\n";
//        }
//        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

//        count1--;
//        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
//        std::cout<<"we are in transmit after count1 decrementation; count1 = "<<count1<<std::endl;
//        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/



//    }



//}
//int transmitter::writing(int socket, char  Buff[], int size)
//{
//    int n = write(socket, Buff, size);
//    return n;
//}
