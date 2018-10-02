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
using namespace std;


extern mutex m;                    //for exclusive access to memory
extern condition_variable cv;      //blocks the calling thread until notified to resume
extern int StartRegister;          //to switch between threads
extern std::set<int64_t> Lines;    //set of Lines belong to this RBC
extern int64_t BlockStatus[29];    //array of status of blocks
extern string p;
//// a spam comment

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
    string path=p+"ListOfLines.txt";           //path to get list of lines from
    string path1 = p+"BlockOccupation.txt";    //path to write outputs inside
    set<int64_t>::iterator iter;               //counter of elements of set Lines
    char dlc[2];                               //to save frameR.can_dlc
    char ss[8];                                //to save frameR.data[i]
    string x;                                  //to save a read line from ListOfLines.
    int64_t l;                                 //to save converted x strings to hex
};

#endif // BLOCKOCCUPATION_H
