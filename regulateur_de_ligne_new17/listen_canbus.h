#ifndef LISTEN_CANBUS_H
#define LISTEN_CANBUS_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <thread>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <vector>

extern std::string p;
extern std::mutex m1;                                    //for exclusive access to memory; between listen_canbus and inputbuffer_can
extern std::condition_variable cv1;                      //blocks the calling thread until notified to resume
extern int can_received;                                 //shows that a frame is received to CAN
extern int in_canbuffer_sign;                            //a sign for transferring the input from listen_canbus to inputbuffer_can
extern struct can_frame frameR;                          //a CAN bus received frame
extern std::vector<struct can_frame> caninputbuffer;     //vector of received CAN frames

class listen_CANbus
{
public:
    listen_CANbus(){}
    void ListenCANBus();

private:
    int s;                                     //socket return value
    int nbytes;                                //read return value
    struct sockaddr_can addr;
    struct ifreq ifr;
    char const *ifname = "can0";               //name of CAN bus
    std::string filename;                           //file name to write outputs inside
    std::string path;                               //path to write outputs inside
    char dlc[2];                               //to save frameR.can_dlc
    char ss[8];                                //to save frameR.data[i]
    std::string x;                                  //to save a read line from ListOfLines.
    int64_t l;                                 //to save converted x strings to hex
    int c;                                     //to save return value of keepcopy
    int64_t trainID;                           //to analyze train ID

};

#endif // LISTEN_CANBUS_H
