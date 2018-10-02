#ifndef BLOCKOCCUPATION_H
#define BLOCKOCCUPATION_H
#include "definitions.h"

#include <set>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <mutex>
#include <condition_variable>


extern std::mutex m5;                        //for exclusive access to memory; between blockoccupation and frame_analysis
extern std::condition_variable cv5;          //blocks the calling thread until notified to resume
extern int StartAnalysis;                    //to notify frame_analysis thread to start
extern int BlockStatus[29];                  //array of status of blocks
extern std::string p;
extern int count_frame;                      //to count size of CANinputbuffer
extern std::mutex m2;                        //for exclusive access to memory; between blockoccupation and inputbuffer_can
extern std::condition_variable cv2;          //blocks the calling thread until notified to resume
extern std::set<int> Lines;                  //set of Lines belong to this RBC

class BlockOccupation
{
public:
    BlockOccupation(){}
    void BlockOccupationFunction();


private:
    int s;                                     //socket return value
    int nbytes;                                //read return value
    struct sockaddr_can addr;
    struct can_frame frameR;
    struct ifreq ifr;
    char const *ifname = "can0";               //name of CAN bus
    std::string path=p+"ListOfLines.txt";           //path to get list of lines from
    std::string path1 = p+"BlockOccupation.txt";    //path to write outputs inside
    std::set<int>::iterator iter;               //counter of elements of set Lines
    char dlc[2];                               //to save frameR.can_dlc
    char ss[8];                                //to save frameR.data[i]
    std::string x;                                  //to save a read line from ListOfLines.
    int l;                                 //to save converted x strings to hex
};

#endif // BLOCKOCCUPATION_H
