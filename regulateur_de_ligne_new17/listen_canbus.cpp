#include "listen_canbus.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <tuple>
#include <mutex>
#include <fstream>
#include <sstream>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include<thread>
#include <linux/can.h>
#include <linux/can/raw.h>
using namespace std;


/***********************************************************************************************************************************************
********************************This function listens to the CAN bus and returns the frame and a modified ID:
*************************************************************************************************************************************************/

void listen_CANbus::ListenCANBus()
{
    filename ="listenCAN";
    path=p+filename+".txt"; //define path for outputing
    ofstream outfile;   //define output file outfile

    /*--------------------------debug---------------------------------------------------------------------------------------------------------------*/
    outfile.open(path,std::ofstream::out | std::ofstream::app);
    outfile<<"\n---*******************>We are in ListenCANBus" <<endl;
    outfile.close();
    /*--------------------------debug---------------------------------------------------------------------------------------------------------------*/

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    /* Prepare socket CAN for reading: */
    if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
        perror("Error while opening socket!");
    }
    strcpy(ifr.ifr_name, ifname);
    ioctl(s, SIOCGIFINDEX, &ifr);
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    printf("%s at index %d\n", ifname, ifr.ifr_ifindex);
    if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Error in socket bind!");
    }
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    /* listen to CAN bus in a loop: */
    while(1)
    {
        {
            std::unique_lock<std::mutex> lk1(m1);

            /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
            /* Read a frame: */
            nbytes = read(s, &frameR, sizeof(struct can_frame));
            if (nbytes < 0)
            {
                perror("can raw socket read");
            }
            if (nbytes < sizeof(struct can_frame))
            {
                fprintf(stderr, "read: incomplete CAN frame\n");
            }
            /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
            in_canbuffer_sign = 1;         //if in_buffer_sign is equal to 1, after notification inputbuffer will be unlocked.
            //std::cout << "CAN frame received. in_canbuffer_sign is "<<in_canbuffer_sign<<std::endl;
        }
        /* notify inputbuffer_can thread to finish waiting and start: */
        cv1.notify_all();

        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
        /* Output the received frame in the thread's file: */
        sprintf(dlc, "%X", frameR.can_dlc);
        outfile.open(path,std::ofstream::out | std::ofstream::app);
        outfile<<"read id : $"<<hex<<frameR.can_id<<" dlc : "<<dlc<<" data : ";
        outfile.close();
        outfile.open(path,std::ofstream::out | std::ofstream::app);
        for(int i=0; i<frameR.can_dlc; i++)
        {
            sprintf(ss, "%X", frameR.data[i]);  //display data
            outfile<<"$"<<ss<<" ";
        }
        outfile.close();
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /*wait till inputbuffer_can thread saves frameR in caninputbuffer and change in_canbuffer_sign to 0: */
        {
            std::unique_lock<std::mutex> lk1(m1);
            while(in_canbuffer_sign) cv1.wait(lk1);
        }
        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/



    }
}


