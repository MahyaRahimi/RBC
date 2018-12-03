#include "outputbuffer.h"
#include <iostream>

void outputbuffer::buffering()
{
    while(1){

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* locks the thread and waits till sign becomes true in a thread. it means that a data vector is ready in frame_to_outputbuffer to be transmitted to train: */
        {
            std::cout<<"here outputbuffer begins; sign: "<<out_buffer_sign<<std::endl;
            std::unique_lock<std::mutex> lk3(m3);
            while(!out_buffer_sign) cv3.wait(lk3);
            /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
          std::cout<<"Here out_buffer_sign in outputbuffer has changed; out_buffer_sign: "<<out_buffer_sign<<std::endl;
            /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

        /* pushes the data to the outputbuffer: */
        myoutputbuffer.push_back(frame_to_outputbuffer);

        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
        std::cout<<"now we show myoutputbuffer: \n";
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
        /* print all the data to be transmitted: */

        for ( int i = 0; i<myoutputbuffer.size(); i++)
        {
           for ( int j = 0; j<myoutputbuffer[i].size(); j++ )
               std::cout << std::hex << int(myoutputbuffer[i][j]) << ' ';
           std::cout << std::endl;
        }
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

        out_buffer_sign = 0;
        }
        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

        /* notify other threads to unlock and continue */
        cv3.notify_all();

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* lock and notify transmitter to start transmitting new outputs: */
        {
            std::lock_guard <std::mutex> lk4(m4);
            /*increments count to signal frame_analysis thread: */
            count1+=1;
            std::cout<<"count1 incremented; count1 = " << count1<< std::endl;
        }
        cv4.notify_one();
        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    }
}
