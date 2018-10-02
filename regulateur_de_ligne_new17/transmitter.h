#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>

extern std::mutex m4;                             //for exclusive access to memory; between output buffer and transmitter
extern std::condition_variable cv4;               //blocks the calling thread until notified to resume
extern int count1;

extern std::vector<std::vector<unsigned char>> myoutputbuffer;    //vector of messages to be transmitted


class transmitter
{
public:
    transmitter(){}
    void transmit();
private:
    std::vector <unsigned char> data_to_transmit;
   // std::vector <int> int_data_to_transmit;

};

#endif // TRANSMITTER_H
