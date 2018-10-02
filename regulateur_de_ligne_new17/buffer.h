#ifndef BUFFER_H
#define BUFFER_H
//#include "listener.h"
//#include "frame_analysis.h"
#include <mutex>
#include <condition_variable>
#include <vector>


extern std::mutex m;                     //for exclusive access to memory
extern std::condition_variable cv;       //blocks the calling thread until notified to resume
extern std::mutex m1;                    //for exclusive access to memory
extern std::condition_variable cv1;      //blocks the calling thread until notified to resume
extern int sign;                     /*a sign for transferring the input to frame_analysis (it is not possible
                                          to define friend classes to transfer sign or myvector)*/
extern std::vector<unsigned int> myvector;           //each element of myvector is a byte of data
extern std::vector<std::vector<unsigned int>> mybuffer;    //vector of received messages
extern int count;                       //to count size of buffer


//class listener;
class buffer
{
    //friend class frame_analysis;
public:
    buffer(){}
    void buffering();
    //listener *l;
};

#endif // BUFFER_H
