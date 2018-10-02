#include "blockoccupation.h"

#include <string.h>
#include <set>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>


/************************************************************************************************************************************************/
/********************************This function saves status of blocks sent by the board "occupation des cantons": *******************************/
/************************************************************************************************************************************************/
void BlockOccupation::BlockOccupationFunction()
{
    std::ifstream ListOfLines;             //define input file ListOfLines. Don't press enter at the last line of ListOfLines file!
    std::ofstream outfile;                 //define output file

    {
    std::unique_lock<std::mutex> lk5(m5);

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    /* Initialize all the blocks to Free */
    for(int64_t i=0;i<29;i++)
    {
        BlockStatus[i]=FREE;
    }
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    /* Read list of lines related to this RBC */
    ListOfLines.open(path);
    /* Send error if coudn't find or open ListOfLines */
    if(!ListOfLines)
    {
        std::cout<<"Error opening ListOfLines!"<<std::endl;
    }
    while(getline(ListOfLines,x))   //get a line from ListOfLines and save to x
    {
        l=stol(x,nullptr,16);       //convert x to hex and save to l
        std::cout<<std::hex<<l<<std::endl;
        Lines.insert(l);            //add l to the set Lines
    }
    ListOfLines.close();
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/


    /*--------------------------debug---------------------------------------------------------------------------------------------------------------*/
    /* print elements of Line: */
    std::cout<<"got these blocks:";
    for(iter = Lines.begin(); iter!=Lines.end(); ++iter)
    {
        std::cout<<*iter<<" ";
        std::cout.flush();
    }
    std::cout<<"\n";
    /*--------------------------debug---------------------------------------------------------------------------------------------------------------*/

    StartAnalysis=1;
    cv5.notify_all();    //to notify frame_analysis thread to unlock and start
    while(StartAnalysis) cv5.wait(lk5);   //to make sure frame_analysis thread is started
    }

    /************************************************************************************************************************************************/
    /* ************************************Listen for saving changes of block statuses: *************************************************************/
    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/


    /*--------------------------debug---------------------------------------------------------------------------------------------------------------*/
    outfile.open(path1,std::ofstream::out | std::ofstream::app);
    outfile<<"\n---*********************We are in thread block occupation"<<std::endl;
    outfile.close();
    /*--------------------------debug---------------------------------------------------------------------------------------------------------------*/

    while(1){

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* locks the thread and waits till count becomes more than one in input buffer thread. it means that there exists something to analyze: */
        std::unique_lock<std::mutex> lk2(m2);
        //std::cout << "now blockoccupation waits for count_frame. count_frame in blockoccupation= "<<count_frame<<std::endl;
        while(!count_frame) cv2.wait(lk2);
        m2.unlock();
        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
        //std::cout<<"we are in blockoccupation after wait; count_frame: "<< count_frame <<std::endl;
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* If the ID was found among related block IDs in Lines file, and if data[0] was 44, data[0] means the status of the block
         * and should be put into BlockStatus[frameR.can_id-29]*/
        if((Lines.find(frameR.can_id) != Lines.end()) && (frameR.data[0]==0x44))
        {
            BlockStatus[frameR.can_id-29]=frameR.data[1];
            std::cout<<"\n*********BlockStatus********** "<<frameR.can_id<<"= "<<BlockStatus[frameR.can_id-29]<<std::endl;
        }
        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
        (count_frame)--;

        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
        //std::cout<<"we are in analysis after count decrementation; count_frame = "<<count_frame<<std::endl;
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
    }

}
