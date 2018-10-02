#ifndef FRAME_ANALYSIS_H
#define FRAME_ANALYSIS_H
#include <mutex>
#include <condition_variable>
#include <vector>
#include <set>

extern std::mutex m;                                            //for exclusive access to memory; between listener and frame_analysis
extern std::condition_variable cv;                              //blocks the calling thread until notified to resume
extern std::mutex m1;                                           //for exclusive access to memory
extern std::condition_variable cv1;                             //blocks the calling thread until notified to resume
extern std::vector<unsigned char> frame_to_process;             //will be sent to process thread; each element of it is a byte of data
extern int processflag;

extern std::vector<unsigned char> frame_to_outputbuffer;        //will be sent to outputbuffer; each element of it is a byte of data
extern int out_buffer_sign;                                     /*a sign for transferring the messages to outputbuffer */
extern std::mutex m3;                                           //for exclusive access to memory; mutex between output buffer and everyone that wants to send out any message
extern std::condition_variable cv3;                             //blocks the calling thread until notified to resume

extern int count;                                               //to count size of inputbuffer
extern std::vector<std::vector<unsigned char>> myinputbuffer;   //vector of received messages

extern int StartAnalysis;                                       //to notify frame_analysis thread to start
extern std::mutex m5;                                           //for exclusive access to memory; between blockoccupation and frame_analysis
extern std::condition_variable cv5;                             //blocks the calling thread until notified to resume
extern int BlockStatus[29];                                     //array of status of blocks
extern std::set<int> Lines;                                     //set of Lines belong to this RBC


class frame_analysis
{
public:
    frame_analysis(){}
    void analysis();
private:
    std::vector <unsigned char> data_to_process;
    std::vector<unsigned char> RBCSystemVersion();
    std::vector<unsigned char> AcknowledgementOfTrainData(unsigned char T_train1, unsigned char T_train2, unsigned char T_train3, unsigned char T_train4, unsigned char NID_LRBG_MSB, unsigned char NID_LRBG_LSB);
    std::vector<unsigned char> MovementAuthority(unsigned char NID_LRBG_MSB, unsigned char NID_LRBG_LSB, unsigned char D_LRBG_MSB, unsigned char D_LRBG_LSB);
    std::vector<unsigned char> EndofAuthorization(int64_t TID);    
    float BalisetoBalise(int b1, int b2);
    std::vector<unsigned char> TerminationOfACommunicationSession(unsigned char NID_ENGINE);

    int message;                                            //ID number of received message to process
    char L_MESSAGE;                                         //lenght of message
    int NID_LRBG;                                           //ID number of balise
    float D_LRBG;                                           //distance to the last group of balise
    int block;                                              //block number
    float v[29];                                            //max speed in each block;
    char N_ITER;                                            //number of sections
    float float_L_SECTION;                                  //integer value of section length
    float BtoB_distance;                                    //balise to balise distance
    float l_bock;                                           //length of block
    std::vector <unsigned char> L_SECTION_LSB;
    std::vector <unsigned char> L_SECTION_MSB;
    std::vector <unsigned char> D_SECTIONTIMERSTOPLOC_LSB;  //D_SECTIONTIMERSTOPLOC = L_SECTION;
    std::vector <unsigned char> D_SECTIONTIMERSTOPLOC_MSB;  //D_SECTIONTIMERSTOPLOC = L_SECTION;
    std::vector <unsigned char> QSECTIONTIMER;
    std::vector <unsigned char> T_SECTIONTIMER;
    //D_SECTIONTIMERSTOPLOC = L_SECTION;
    //L_ENDSECTION = L_SECTION [N]
    char QENDTIMER = 0;
    char T_ENDTIMER = 0xFF;
    char D_ENDTIMERSTARTLOC_LSB = 0xFF;
    char D_ENDTIMERSTARTLOC_MSB = 0xFF;
    char QDANGERPOINT = 0;
    char D_DP = 0;


};

#endif // FRAME_ANALYSIS_H
