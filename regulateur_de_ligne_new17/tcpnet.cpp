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

