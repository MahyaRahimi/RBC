#include "blockoccupation.h"

#include <string.h>
#include <set>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

/************************************************************************************************************************************************/
/********************************This function saves status of blocks sent by the board "occupation des cantons": *******************************/
/************************************************************************************************************************************************/
void BlockOccupation::BlockOccupationFunction()
{
    ifstream ListOfLines;             //define input file ListOfLines. Don't press enter at the last line of ListOfLines file!
    ofstream outfile;                 //define output file

    {
    unique_lock<mutex> lk(m);

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
        cout<<"Error opening ListOfLines!"<<endl;
    }
    while(getline(ListOfLines,x))   //get a line from ListOfLines and save to x
    {
        l=stol(x,nullptr,16);       //convert x to hex and save to l
        cout<<hex<<l<<endl;
        Lines.insert(l);            //add l to the set Lines
    }
    ListOfLines.close();
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/


    /*--------------------------debug---------------------------------------------------------------------------------------------------------------*/
    cout<<*Lines.begin()<<"here"<<*Lines.end()<<endl;

    /* print elements of Line: */
    for(iter = Lines.begin(); iter!=Lines.end(); ++iter)
    {
        cout<<*iter;
        cout.flush();
    }
    /*--------------------------debug---------------------------------------------------------------------------------------------------------------*/

    StartRegister=1;
    cv.notify_all();    //to notify registration thread to unlock and start
    while(StartRegister) cv.wait(lk);   //to make sure registration thread is started
    }

    /************************************************************************************************************************************************/
    /* ************************************Listen for saving changes of block statuses: *************************************************************/
    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/


    /*--------------------------debug---------------------------------------------------------------------------------------------------------------*/
    outfile.open(path1,std::ofstream::out | std::ofstream::app);
    outfile<<"\n---*********************We are in thread block occupation"<<endl;
    outfile.close();
    /*--------------------------debug---------------------------------------------------------------------------------------------------------------*/

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    /* Prepare socket CAN for reading: */
    if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
        perror("Error while opening socket");
    }
    strcpy(ifr.ifr_name, ifname);
    ioctl(s, SIOCGIFINDEX, &ifr);
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    printf("%s at index %d\n", ifname, ifr.ifr_ifindex);
    if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Error in socket bind");
    }
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/


    while(1)  //listen in a true while loop
    {
        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* Read a frame from CAN */
        cout<<"block occupation is waiting for receiving a frame:"<<endl;
        nbytes = read(s, &frameR, sizeof(struct can_frame));
        if (nbytes < 0) //if error in reading
        {
            perror("can raw socket read");
        }
        if (nbytes < sizeof(struct can_frame))  //if error in reading
        {
            fprintf(stderr, "read: incomplete CAN frame\n");
        }
        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

        /*--------------------------debug---------------------------------------------------------------------------------------------------------------*/
        /* Display the received frame in file BlockOccupation.txt*/
        sprintf(dlc, "%X", frameR.can_dlc);
        outfile.open(path1,std::ofstream::out | std::ofstream::app);
        outfile<<"\nread id : $"<<hex<<frameR.can_id<<" dlc : "<<dlc<<" data : ";
        outfile.close();

        outfile.open(path1,std::ofstream::out | std::ofstream::app);
        for(int i=0; i<frameR.can_dlc; i++)
        {
            sprintf(ss, "%X", frameR.data[i]);  //display data
            outfile<<"$"<<ss<<" ";
        }
        outfile.close();
        /*--------------------------debug---------------------------------------------------------------------------------------------------------------*/

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* If the ID was found among related block IDs in Lines file, and if data[0] was 44, data[0] means the status of the block
         * and should be put into BlockStatus[frameR.can_id-29]*/
        if((Lines.find(frameR.can_id) != Lines.end()) && (frameR.data[0]==0x44))
        {
            BlockStatus[frameR.can_id-29]=frameR.data[1];
            cout<<"\n*********BlockStatus********** "<<frameR.can_id<<"= "<<BlockStatus[frameR.can_id-29]<<endl;
        }
        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    }

}
