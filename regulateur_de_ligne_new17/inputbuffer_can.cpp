#include "inputbuffer_can.h"

#include <iostream>
extern std::vector<struct can_frame> caninputbuffer;    //vector of received CAN frames

/************************************************************************************************************************************************/
/*****************This function makes a queue of received frames in CAN bus, and sends them to blockoccupation **********************************
 **********************************one by one to see if they are related to block occupation *****************************************************
************************************************************************************************************************************************/

void inputbuffer_can::buffering_can()
{
    while(1){

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* locks the thread and waits till in_canbuffer_sign becomes true in listen_canbus thread. it means that all data is received and saved in mycanvector: */
        {
            //std::cout<<"here inputbuffer_can begins; in_canbuffer_sign: "<<in_canbuffer_sign<<std::endl;
            std::unique_lock<std::mutex> lk1(m1);
            while(!in_canbuffer_sign) cv1.wait(lk1);
            /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
           // std::cout<<"here in_buffer_sign in inputbuffer has changed; in_buffer_sign: "<<in_canbuffer_sign<<std::endl;
            /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

        /* pushes the data in the inputbuffer: */
        caninputbuffer.push_back(frameR);

        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
      //  std::cout<<"now we show caninputbuffer: \n";
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
        /* print all the data received: */
       /* for ( int i = 0; i<caninputbuffer.size(); i++)
        {
            sprintf(dlc, "%X", caninputbuffer[i].can_dlc);
            std::cout<<"read id : $"<<std::hex<<caninputbuffer[i].can_id<<" dlc : "<<dlc<<" data : ";
            for(int j=0; j<caninputbuffer[i].can_dlc; j++)
            {
                sprintf(ss, "%X", caninputbuffer[i].data[j]);  //display data
                std::cout<<"$"<<ss<<" ";
            }
        }*/
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/


        in_canbuffer_sign = 0;
        }
        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

        /* notify listener to unlock and wait for another input */
        cv1.notify_all();

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* lock and notify blockoccupation to start analysis of new inputs: */
        {
            std::lock_guard <std::mutex> lk2(m2);
            /* increments count to signal frame_analysis thread: */
            count_frame+=1;
           // std::cout<<"count_frame in CAN input buffer incremented; count_frame = " << count_frame<< std::endl;
        }
        cv2.notify_all();
        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    }
}
