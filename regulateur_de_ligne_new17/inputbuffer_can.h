#ifndef INPUTBUFFER_CAN_H
#define INPUTBUFFER_CAN_H
#include <mutex>
#include <condition_variable>
#include <vector>
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

extern std::mutex m1;                          //for exclusive access to memory; between listen_canbus and inputbuffer_can
extern std::condition_variable cv1;            //blocks the calling thread until notified to resume
extern int in_canbuffer_sign;           //a sign for transferring the input from listen_canbus to inputbuffer_can
extern struct can_frame frameR;         //a CAN bus received frame
extern std::vector<struct can_frame> caninputbuffer;    //vector of received CAN frames
extern int count_frame;                       //to count size of CANinputbuffer
extern std::mutex m2;                          //for exclusive access to memory; between blockoccupation and inputbuffer_can
extern std::condition_variable cv2;            //blocks the calling thread until notified to resume

class inputbuffer_can
{
public:
    inputbuffer_can(){}
    void buffering_can();
private:
    char dlc[2];                               //to save frameR.can_dlc
    char ss[8];                                //to save frameR.data[i]

};

#endif // INPUTBUFFER_CAN_H
