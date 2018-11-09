#include "inputbuffer.h"

#include <iostream>
#include <thread>
#include <chrono>

/************************************************************************************************************************************************/
/*****************This function makes a queue of received frames, and sends them to frame_analysis one by one analyzes them one by one and sends to related threads**************************/
/************************************************************************************************************************************************/


void inputbuffer::buffering(){
    while(1){

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* locks the thread and waits till in_buffer_sign becomes true in listener thread. it means that all data is received and saved in myvector: */
        {
            std::cout<<"here inputbuffer begins; in_buffer_sign: "<<in_buffer_sign<<std::endl;
            std::unique_lock<std::mutex> lk(m10);
            std::cout<<"next step inputbuffer will wait; in_buffer_sign: "<<in_buffer_sign<<std::endl;

            while(!in_buffer_sign) cv10.wait(lk);

            /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
            std::cout<<"here in_buffer_sign in inputbuffer has changed; in_buffer_sign: "<<in_buffer_sign<<std::endl;
            /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

            /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
            std::cout << "vector in inputbuffer is: "<<std::endl;
            int k=0;
            for (auto v : myvector)
            {
                    std::cout << k << ":" << std::hex << int(v) << "\n";
                    k++;
            }
            /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

            /* pushes the data in the inputbuffer: */
            myinputbuffer.push_back(myvector);


            /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
            std::cout<<"now we show myinputbuffer: \n";
            /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

            /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
            /* print all the data received: */

            for ( int i = 0; i<myinputbuffer.size(); i++)
            {
               for ( int j = 0; j<myinputbuffer[i].size(); j++ )
                   std::cout << std::hex << int(myinputbuffer[i][j]) << ' ';

               std::cout << std::endl;
            }

            /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

            in_buffer_sign = 0;

            /* notify listener to unlock and wait for another input */
            cv10.notify_all();
        }
        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

        ///* notify listener to unlock and wait for another input */
        //cv10.notify_all();

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* lock and notify frame_analysis to start analysis of new inputs: */
        {
            std::unique_lock<std::mutex> lk(m);
            /* increments count to signal frame_analysis thread: */
            count+=1;
            std::cout<<"count incremented; count = " << count<< std::endl;
        }
        cv.notify_all();
        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    }
}
