#include "listener.h"
#include <iostream>
#include <vector>


/************************************************************************************************************************************************/
/********************************This function ideally listens, receives the frames and sends them to inputbuffer********,****************************/
/*******************Since we don't have correct XBee listen funtion, we simulate frame reception in another way : *******************************/
/************************************************************************************************************************************************/



void listener::listen()
{
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
