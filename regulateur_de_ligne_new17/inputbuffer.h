#ifndef INPUTBUFFER_H
#define INPUTBUFFER_H

#include <mutex>
#include <condition_variable>
#include <vector>


extern std::mutex m;                     //for exclusive access to memory, m is between listener and inputbuffer
extern std::condition_variable cv;       //blocks the calling thread until notified to resume
extern std::mutex m1;                    //for exclusive access to memory
extern std::condition_variable cv1;      //blocks the calling thread until notified to resume
extern int in_buffer_sign;                     /*a sign for transferring the input to input buffer (it is not possible
                                          to define friend classes to transfer in_buffer_sign or myvector)*/
extern std::vector<unsigned char> myvector;           //recieved from listener; each element of myvector is a byte of data
extern std::vector<std::vector<unsigned char>> myinputbuffer;    //vector of received messages
extern int count;                       //to count size of inputbuffer


class inputbuffer
{
public:
    inputbuffer(){}
    void buffering();
};

#endif // INPUTBUFFER_H
