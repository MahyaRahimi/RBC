#ifndef OUTPUTBUFFER_H
#define OUTPUTBUFFER_H

#include <mutex>
#include <condition_variable>
#include <vector>


extern std::mutex m3;                     //for exclusive access to memory; mutex between output buffer and everyone that wants to send out any message
extern std::condition_variable cv3;       //blocks the calling thread until notified to resume
extern int out_buffer_sign;                        /*a sign for transferring the messages to outputbuffer */

extern std::mutex m4;                             //for exclusive access to memory; between output buffer and transmitter
extern std::condition_variable cv4;               //blocks the calling thread until notified to resume
extern int count1;

extern std::vector<std::vector<unsigned char>> myoutputbuffer;    //vector of messages to be transmitted
extern std::vector<unsigned char> frame_to_outputbuffer;        //is received from CommuEstabl; each element of it is a byte of data


class outputbuffer
{
public:
    outputbuffer(){}
    void buffering();
};

#endif // OUTPUTBUFFER_H
