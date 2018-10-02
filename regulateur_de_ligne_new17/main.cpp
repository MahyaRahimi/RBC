/* There is no keepcopy function in this version. We don't keep a copy of each listened frame.
 * Data is cin from the user in int format, but is converted to unsigned char when saving analysing. otherwise we
 * cannot enter numbers like 120 and save all the digits (only first digit will be saved
 * In this version whoole mesage processing and demand answering is in one thread. */

#include "definitions.h"
#include "listener.h"
#include "frame_analysis.h"
#include "inputbuffer.h"
#include "inputbuffer_can.h"
#include "outputbuffer.h"
#include "transmitter.h"
#include "listen_canbus.h"
#include "blockoccupation.h"

#include<thread>
#include <iostream>
#include <set>
/*#include <string.h>
#include <tuple>
#include <mutex>
#include <condition_variable>*/

std::mutex m;                                                    //for exclusive access to memory
std::condition_variable cv;                                      //blocks the calling thread until notified to resume
std::mutex m1;                                                   //for exclusive access to memory; between listen_canbus and inputbuffer_can
int in_canbuffer_sign;                                           //a sign for transferring the input from listen_canbus to inputbuffer_can
std::condition_variable cv1;                                     //blocks the calling thread until notified to resume
struct can_frame frameR;                                  //a CAN bus received frame
int in_buffer_sign = 0;                                          //a sign for transferring the input to frame_analysis
std::vector<unsigned char> myvector = {};                        //each element of myvector is a byte of data
std::vector<std::vector<unsigned char>> myinputbuffer = {};      //vector of received messages
int count = 0;                                                   //to count size of inputbuffer
std::vector<unsigned char> frame_to_process;                     //will be sent to process thread; each element of it is a byte of data
int processflag = 0;                                             //to notify process thread
std::mutex m4;                                                   //for exclusive access to memory; between output buffer and transmitter
std::condition_variable cv4;                                     //blocks the calling thread until notified to resume
int count1;
std::vector<unsigned char> frame_to_outputbuffer;                //will be sent to outputbuffer; each element of it is a byte of data
int out_buffer_sign;                                             // a sign for transferring the messages to outputbuffer
std::mutex m3;                                                   //for exclusive access to memory; mutex between output buffer and everyone that wants to send out any message
std::condition_variable cv3;                                     //blocks the calling thread until notified to resume
std::vector<std::vector<unsigned char>> myoutputbuffer;          //vector of messages to be transmitted
int count_frame;                                                 //to count size of CANinputbuffer
std::vector<struct can_frame> caninputbuffer;                    //vector of received CAN frames
std::mutex m2;                                                   //for exclusive access to memory; between blockoccupation and inputbuffer_can
std::condition_variable cv2;                                     //blocks the calling thread until notified to resume
std::string p="/home/ubuntu/QT_workspace/regulateur_de_ligne_new17/";    //path for in/output
std::mutex m5;                                                   //for exclusive access to memory; between blockoccupation and frame_analysis
std::condition_variable cv5;                                     //blocks the calling thread until notified to resume
int StartAnalysis;                                               //to notify frame_analysis thread to start
std::set<int> Lines;                                          //set of Lines belong to this RBC
int BlockStatus[29];                                             //array of status of blocks


int main()
{

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    /* create listener thread: */
    listener objlistener;
    std::thread threadlistener(&listener::listen, objlistener);//creates thread threadlistener that calls listener()
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    /* create listen_canbus thread: */
    listen_CANbus objlistencanbus;
    std::thread threadlisten_canbus(&listen_CANbus::ListenCANBus, objlistencanbus);//creates thread threadlisten_canbus that calls ListenCANBus()
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    /* create transmitter thread : */
    transmitter objtransmit;
    std::thread threadtransmit(&transmitter::transmit, objtransmit);//creates thread threadtransmit that calls transmit()
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    /* create inputbuffer thread: */
    inputbuffer objinputbuffer;
    std::thread threadinputbuffer (&inputbuffer::buffering, objinputbuffer);//creates thread threadinputbuffer that calls buffering()
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    /* create inputbuffer_can thread: */
    inputbuffer_can objinputbuffer_can;
    std::thread threadinputbuffer_can (&inputbuffer_can::buffering_can, objinputbuffer_can);//creates thread threadinputbuffer that calls buffering()
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    /* create outputbuffer thread: */
    outputbuffer objoutputbuffer;
    std::thread threadoutputbuffer (&outputbuffer::buffering, objoutputbuffer);//creates thread threadoutputbuffer that calls buffering()
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    /* create frame_analysis thread: */
    frame_analysis objanalysis;
    std::thread threadanalysis (&frame_analysis::analysis, objanalysis);//creates thread threadanalysis that calls analysis()
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    /* create block_occupation thread: */
    BlockOccupation objblockoccu;
    std::thread threadblockoccupation (&BlockOccupation::BlockOccupationFunction, objblockoccu);//creates thread threadblockoccupation that calls BlockOccupationFunction()
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    /* thread join */
    threadlistener.join();//pauses until threadlistener finishes
    threadlisten_canbus.join();
    threadinputbuffer.join();
    threadinputbuffer_can.join();
    threadoutputbuffer.join();
    threadanalysis.join();
    threadtransmit.join();
    threadblockoccupation.join();
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    return 0;
}
