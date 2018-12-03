#include "transmitter.h"
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
/********************************This function ideally receives the frames from the output buffer and transmits them to trains *************************/
/*******************Since we don't have correct XBee transmitter funtion, we simulate frame transmition in another way : ************************/
/************************************************************************************************************************************************/

void transmitter::transmit()
{
    while(1){
        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* locks the thread and waits till count1 becomes more than one in output buffer thread. it means that there exists something to transmit: */
        std::unique_lock<std::mutex> lk4(m4);
        std::cout << "now transmitter waits for count1. count1 in transmitter= "<<count1<<std::endl;
        while(!count1) cv4.wait(lk4);
        m4.unlock();
        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
        std::cout<<"we are in transmitter after wait; count1: "<< count1 <<std::endl;
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /****************** start transmission of outnputs to trains: ************************************************************************************************************************/
        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

        /* retreive data from the vector myoutputbuffer: */
        data_to_transmit = myoutputbuffer.front();

        /* to delete the retreived data from the myoutputbuffer: */
        myoutputbuffer.erase(myoutputbuffer.begin());

        /* send messages to the client via TCP: */
        memset(sendBuff, 0, sizeof(sendBuff));
        strcpy(sendBuff, "Message from server in transmitter\n");
        writing(connfd, sendBuff, strlen(sendBuff));

        memset(sendBuff, 0, sizeof(sendBuff));
        std::copy(data_to_transmit.begin(), data_to_transmit.end(), sendBuff);
        writing(connfd, sendBuff, strlen(sendBuff));

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* print the message to be transmitted; instead of sending by XBEE artificially we print the message: */
        std::cout << "\n\n\n******\nHere is the message to the train: \n******\n\n\n";
        //int_data_to_transmit = data_to_transmit;
        for ( int i = 0; i<data_to_transmit.size(); i++ ){
            //int_data_to_transmit[i] = data_to_transmit[i];
            std::cout << std::hex << int(data_to_transmit[i]) << "\n";
        }
        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

        count1--;
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
        std::cout<<"we are in transmit after count1 decrementation; count1 = "<<count1<<std::endl;
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/



    }



}
int transmitter::writing(int socket, char  Buff[], int size)
{
    int n = write(socket, Buff, size);
    return n;
}
