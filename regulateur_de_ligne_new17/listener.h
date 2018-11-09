#ifndef LISTENER_H
#define LISTENER_H
#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>

extern std::mutex m;                    //for exclusive access to memory, m is between listener and inputbuffer
extern std::condition_variable cv;      //blocks the calling thread until notified to resume
extern std::mutex m10;                    //for exclusive access to memory, m10 is between listener and inputbuffer
extern std::condition_variable cv10;      //blocks the calling thread until notified to resume

extern int in_buffer_sign;                        /*a sign for transferring the input to inputbuffer (it is not possible
                                             to define friend classes to transfer in_buffer_sign or myvector)*/
extern std::vector<unsigned char> myvector;      //each element of myvector is a byte of data

class listener
{
public:
    listener(){}
    int listening();
private:
    int myint;                 //input data
    unsigned char mychar;               //input data transformed to char
    void error_server(const char *msg);
    void dostuff (int sock);
};

#endif // LISTENER_H
