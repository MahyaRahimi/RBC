#include "frame_analysis.h"
#include "definitions.h"

#include <iostream>


/************************************************************************************************************************************************/
/*****************This function makes a queue of received frames, analyzes them one by one and sends to related threads**************************/
/************************************************************************************************************************************************/

void frame_analysis::analysis(){
    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    /* Lock the function and wait until block occupation initializes all the blocks: */
    {
        std::unique_lock<std::mutex> lk5(m5);
        while(!StartAnalysis) cv5.wait(lk5);
        /* Notify block occupation that RegisterFunction has resumed its operation: */
        StartAnalysis = 0;
    }
    cv5.notify_all();  //notifies blockoccupation that I'm unlocked and I continue
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
    while(1){

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* locks the thread and waits till count becomes more than one in input buffer thread. it means that there exists something to analyze: */
        std::unique_lock<std::mutex> lk(m);
        std::cout << "now analysis waits for count. count in analysis= "<<count<<std::endl;
        while(!count) cv.wait(lk);
        m.unlock();
        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
        std::cout<<"we are in analysis after wait; count: "<< count <<std::endl;
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /****************** start analysing and distributing inputs to other functions: ******************************************************************************************************/
        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

        /* retreive data from the vector myinputbuffer: */
        data_to_process = myinputbuffer.front();

        /* to delete the retreived data from the vector: */
        myinputbuffer.erase(myinputbuffer.begin());

        int message = data_to_process.front();           //ID number of received message to process

        /* check the corresponding function to send and process the data: */
        switch (message) {
        case 155://Message 155 = Initiation of a communication session
            /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
            /* lock the thread to save message in frame_to_outputbuffer for sending to the buffer*/
            std::cout<< "155 entered\n";

            {
            std::unique_lock<std::mutex> lk3(m3);
            frame_to_outputbuffer = RBCSystemVersion();
            out_buffer_sign = 1;
            }
            cv3.notify_all(); //notify output buffer to push
            /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

            /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
            /*wait till outputbuffer thread saves frame_to_outputbuffer in myoutputbuffer and change out_buffer_sign to 0: */
            {
                std::unique_lock<std::mutex> lk3(m3);
                while(out_buffer_sign) cv3.wait(lk3);
            }
            /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
            break;
        case 159://Message 159 = Session established
            /** if this message is received, RBC should unlock the trains thread to follow it (if it is needed to follow the train) and also
                     extract the IP address of the train and the senders**/
             std::cout<< "159 entered\n";
            break;
        case 129://Message 129 = Validated train data
            std::cout<< "129 entered\n";

            /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
            /* lock the thread to save message in frame_to_outputbuffer for sending to the buffer*/
            {
            std::unique_lock<std::mutex> lk3(m3);
            frame_to_outputbuffer = AcknowledgementOfTrainData(data_to_process.at(3),
                                                               data_to_process.at(4),
                                                               data_to_process.at(5),
                                                               data_to_process.at(6),
                                                               data_to_process.at(11),
                                                               data_to_process.at(12));
            out_buffer_sign = 1;
            }
            cv3.notify_all(); //notify output buffer to push
            /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

            /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
            /*wait till outputbuffer thread saves frame_to_outputbuffer in myoutputbuffer and change out_buffer_sign to 0: */
            {
                std::unique_lock<std::mutex> lk3(m3);
                while(out_buffer_sign) cv3.wait(lk3);
            }
            /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
            break;
        case 132://message 132 = MA request
            std::cout<< "132 entered\n";
            
            /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
            /* lock the thread to save message in frame_to_outputbuffer for sending to the buffer*/
            {
            std::unique_lock<std::mutex> lk3(m3);
            frame_to_outputbuffer = MovementAuthority(data_to_process.at(12),
                                                      data_to_process.at(13),
                                                      data_to_process.at(14),
                                                      data_to_process.at(15));
            out_buffer_sign = 1;
            }
            cv3.notify_all(); //notify output buffer to push
            /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

            /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
            /*wait till outputbuffer thread saves frame_to_outputbuffer in myoutputbuffer and change out_buffer_sign to 0: */
            {
                std::unique_lock<std::mutex> lk3(m3);
                while(out_buffer_sign) cv3.wait(lk3);
            }
            /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
            break;
        case 150://message 150 = End of mission
            std::cout<< "150 entered\n";

            /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
            /* lock the thread to save message in frame_to_outputbuffer for sending to the buffer*/
            {
            std::unique_lock<std::mutex> lk3(m3);
            frame_to_outputbuffer = TerminationOfACommunicationSession(data_to_process.at(6));
            out_buffer_sign = 1;
            }
            cv3.notify_all(); //notify output buffer to push
            /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

            /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
            /*wait till outputbuffer thread saves frame_to_outputbuffer in myoutputbuffer and change out_buffer_sign to 0: */
            {
                std::unique_lock<std::mutex> lk3(m3);
                while(out_buffer_sign) cv3.wait(lk3);
            }
            /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
            break;
        case 146://message 146 = Acknowledgement
            std::cout<< "146 entered\n";
        /** What should RBC do with this acknowledgement? should it wait for the acknowledgement of the train and if didn't received it, RBC should send again
        TerminationOfACommunicationSession message?????????? what is its exact process? **/
            break;
        case 156://message 156 = Termination of a communication session
            std::cout<< "156 entered\n";
            //I don't implement it yet, since RBC should send NID_LRBG to the train while it is not saved in RBC and I don't know even when this message will be used.
            break;
        default:
            std::cout << "message ID is: " << int(message) << std::endl;
            break;
        }



        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

        (count)--;
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
        std::cout<<"we are in analysis after count decrementation; count = "<<count<<std::endl;
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/


    }

}
std::vector<unsigned char> frame_analysis::RBCSystemVersion()
{
    std::vector<unsigned char> system_version;
    system_version.push_back(32);  //NID_MESSAGE
    system_version.push_back(10); //L_MESSAGE
    std::time_t t = std::time(0); //timestamp of PC
    unsigned char t1 = t %256;  //??should we treat data as they are hex? if not, 256 should be changed to 100
     t = t / 256;
    unsigned char t2 = t % 256;
     t = t / 256;
    unsigned char t3 = t % 256;
    unsigned char t4 = t / 256;
    system_version.push_back(t4); //T_TRAIN
    system_version.push_back(t3); //T_TRAIN
    system_version.push_back(t2); //T_TRAIN
    system_version.push_back(t1); //T_TRAIN
    system_version.push_back(1); //M_ACK
    system_version.push_back(0xFF); //MSB of NID_LRBG (unknown)
    system_version.push_back(0xFF); //LSB of NID_LRBG (unknown)
    system_version.push_back(0x21); //M_VERSION (00100001)
    return system_version;
}

std::vector<unsigned char> frame_analysis::AcknowledgementOfTrainData(unsigned char T_train1, unsigned char T_train2, unsigned char T_train3, unsigned char T_train4, unsigned char NID_LRBG_MSB, unsigned char NID_LRBG_LSB)
{
    std::vector<unsigned char> Acknowledgement;
    Acknowledgement.push_back(8);   //NID_MESSAGE
    Acknowledgement.push_back(13);  //L_MESSAGE
    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    /* Compute T_TRAIN: */
    std::time_t t = std::time(0); //timestamp of RBC
    unsigned char t1 = t %256;  //??should we treat data as they are hex? if not, 256 should be changed to 100
     t = t / 256;
    unsigned char t2 = t % 256;
     t = t / 256;
    unsigned char t3 = t % 256;
    unsigned char t4 = t / 256;
    Acknowledgement.push_back(t4);
    Acknowledgement.push_back(t3);
    Acknowledgement.push_back(t2);
    Acknowledgement.push_back(t1);
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
    Acknowledgement.push_back(0);
    Acknowledgement.push_back(NID_LRBG_MSB);
    Acknowledgement.push_back(NID_LRBG_LSB);
    Acknowledgement.push_back(T_train1);
    Acknowledgement.push_back(T_train2);
    Acknowledgement.push_back(T_train3);
    Acknowledgement.push_back(T_train4);

    return Acknowledgement;
}

std::vector<unsigned char> frame_analysis::MovementAuthority(unsigned char NID_LRBG_MSB, unsigned char NID_LRBG_LSB, unsigned char D_LRBG_MSB, unsigned char D_LRBG_LSB)
{
    std::vector<unsigned char> MA; //movement authority message
    int EOA;                      //End of authority block
    int L_PACKET_15;
    char L_PACKET_15_MSB;
    char L_PACKET_15_LSB;
    int L_PACKET_80;
    char L_PACKET_80_MSB;
    char L_PACKET_80_LSB;
    L_SECTION_LSB.clear();
    L_SECTION_MSB.clear();
    D_SECTIONTIMERSTOPLOC_LSB.clear();
    D_SECTIONTIMERSTOPLOC_MSB.clear();
    QSECTIONTIMER.clear();
    T_SECTIONTIMER.clear();

    MA.push_back(3); //NID_MESSAGE

    /*Compute lenght of message - for this issue, we should define how many sections are available after the train. so should determine EOA.
      For this issue we should first determine block ID in which the train exist. */
    
    /* initiate max speed in each lock; Default max speed of train is fixed. Whereas we can define a specific speed for each block. */
    for (int i=0; i<29; i++)
        v[i] = velocity;

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    /* Find the Id of block in which the train is and float_L_SECTION as L_section.at(0): */
    D_LRBG = D_LRBG_MSB * 256 + D_LRBG_LSB;  // make D_LRBG
    NID_LRBG = NID_LRBG_MSB * 256 + NID_LRBG_LSB;   // make NID_LRBG

    if (D_LRBG <D_Balise_endof_Block)   //if train is located after the balise and before end of the block
    {
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
        std::cout << "the train is located after the balise and before end of the block\n";
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

        switch (NID_LRBG) {
        case 0x3F2:
            block = C1;
            break;
        case 0x3D2:
            block = C2;
            break;
        case 0x3CA:
            block = C3;
            break;
        case 0x3A2:
            block = C4;
            break;
        case 0x39A:
            block = C5;
            break;
        case 0x37A:
            block = C6;
            break;
        case 0x342:
            block = C7;
            break;
        case 0x302:
            block = C8;
            break;
        case 0x30A:
            block = C9;
            break;
        case 0x3DA:
            block = C10;
            break;
        case 0x3C2:
            block = C11;
            break;
        case 0x3AA:
            block = C12;
            break;
        case 0x372:
            block = C13;
            break;
        case 0x352:
            block = C14;
            break;
        case 0x34A:
            block = C15;
            break;
        case 0x312:
            block = C16;
            break;
        case 0x3E2:
            block = C17;
            break;
        case 0x392:
            block = C18;
            break;
        case 0x35A:
            block = C19;
            break;
        case 0x33A:
            block = C20;
            break;
        case 0x31A:
            block = C21;
            break;
        case 0x322:
            block = C22;
            break;
        case 0x32A:
            block = C23;
            break;
        case 0x332:
            block = C24;
            break;
        case 0x362:
            block = C25;
            break;
        case 0x38A:
            block = C26;
            break;
        case 0x3B2:
            block = C27;
            break;
        case 0x3BA:
            block = C28;
            break;
        case 0x3EA:
            block = C29;
            break;
        default:
            std::cout<<"incorrect NID_LRBG"<<std::endl;
            break;
        }
        float_L_SECTION = D_Balise_endof_Block - D_LRBG;

    } else { //if the train is located before the balise in the block
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
        std::cout << "the train is located before the balise in the block\n";
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

        switch (NID_LRBG) {
        case 0x3F2:
            block = C2;
            BtoB_distance = B1_B2;
            break;
        case 0x3D2:
            block = C3;
            BtoB_distance = B2_B3;
            break;
        case 0x3CA:
            block = C4;
            BtoB_distance = B3_B4;
            break;
        case 0x3A2:
            block = C5;
            BtoB_distance = B4_B5;
            break;
        case 0x39A:
            block = C6;
            BtoB_distance = B5_B6;
            break;
        case 0x37A:
            block = C7;
            BtoB_distance = B6_B7;
            break;
        case 0x342:
            block = C8;
            BtoB_distance = B7_B8;
            break;
        case 0x302:
            block = C9;
            BtoB_distance = B8_B9;
            break;
        case 0x30A:
            block = *Lines.begin(); /*the next block after C9, where balise with ID 0x30A is placed, might be C1 or C22 depending to the RBC.
                                    It's the train or Node regulator's duty to call the related RBC to start following the train depending
                                    to the itinary. If RBC_L4 should follow, block would be C22, if RBC_L1 should follow, block would be C1.
                                    Otherwise the train should stay in the node and take C10 or C17 */
            if (*Lines.begin() == C1)
                BtoB_distance = B9_B1;
            else if (*Lines.begin() == C22)
                BtoB_distance = B9_B22;
            break;
        case 0x3DA:
            block = C11;
            BtoB_distance = B10_B11;
            break;
        case 0x3C2:
            block = C12;
            BtoB_distance = B11_B12;
            break;
        case 0x3AA:
            block = C18; /*the next block after C12, where balise with ID 0x3AA is placed, might be C13 or C18 depending to the RBC. It's the
                            train or Node regulator's duty to call the related RBC to start following the train depending to the itinary. If RBC_L5
                            should follow, block would be C18, if RBC_N2 should follow, block would be C13; whereas blocks that belong to node
                            regulators are managed by a node regulator program not a line regulator program.*/
            BtoB_distance = B12_B18;
            break;
        case 0x372:
            block = *Lines.begin(); /*the next block after C13, where balise with ID 0x372 is placed, might be C14 or C7 depending to the RBC.
                                    It's the train or Node regulator's duty to call the related RBC to start following the train depending
                                    to the itinary. If RBC_L3 should follow, block would be C7, if RBC_L6 should follow, block would be C14.*/
            if (*Lines.begin() == C7)
                BtoB_distance = B13_B7;
            else if (*Lines.begin() == C14)
                BtoB_distance = B13_B14;
            break;
        case 0x352:
            block = C15;
            BtoB_distance = B14_B15;
            break;
        case 0x34A:
            block = C16;
            BtoB_distance = B15_B16;
            break;
        case 0x312:
            block = *Lines.begin(); /*the next block after C16, where balise with ID 0x312 is placed, might be C1 or C22 depending to the RBC.
                                    It's the train or Node regulator's duty to call the related RBC to start following the train depending
                                    to the itinary. If RBC_L4 should follow, block would be C22, if RBC_L1 should follow, block would be C1.
                                    Otherwise the train should stay in the node and take C10 or C17 */
            if (*Lines.begin() == C1)
                BtoB_distance = B16_B1;
            else if (*Lines.begin() == C22)
                BtoB_distance = B16_B22;
            break;
        case 0x3E2:
            block = C11;
            BtoB_distance = B17_B11;
            break;
        case 0x392:
            block = C19;
            BtoB_distance = B18_B19;
            break;
        case 0x35A:
            block = C20;
            BtoB_distance = B19_B20;
            break;
        case 0x33A:
            block = C21;
            BtoB_distance = B20_B21;
            break;
        case 0x31A:
            block = C22; /*the next block after C21, where balise with ID 0x31A is placed, might be C17 or C22 depending to the RBC. It's the
                         train or Node regulator's duty to call the related RBC to start following the train depending to the itinary. If RBC_L4
                         should follow, block would be C22, if RBC_N1 should follow, block would be C17; whereas blocks that belong to node
                         regulators are managed by a node regulator program not a line regulator program.*/
            BtoB_distance = B21_B22;
            break;
        case 0x322:
            block = C23;
            BtoB_distance = B22_B23;
            break;
        case 0x32A:
            block = C24;
            BtoB_distance = B23_B24;
            break;
        case 0x332:
            block = C25;
            float_L_SECTION =  B24_B25 - D_LRBG + B25_B250 + D_Balise_endof_Block;  //distance to the end of the block 25
            break;
        case 0x362: //inverse balise, whereas in configuration 4 there is only one direction
            block = C26;
            float_L_SECTION =  B250_B26 - D_LRBG + B26_B251 + D_Balise_endof_Block;  //distance to the end of the block 26
            break;
        case 0x38A: //inverse balise, whereas in configuration 4 there is only one direction
            block = C27;
            BtoB_distance = B251_B27;
            break;
        case 0x3B2:
            block = C28;
            BtoB_distance = B27_B28;
            break;
        case 0x3BA:
            block = C29;
            BtoB_distance = B28_B29;
            break;
        case 0x3EA:
            block = C11;
            BtoB_distance = B29_B11;
            break;
        default:
            std::cout<<"incorrect NID_LRBG or it's equal to 0X36A or 0X382"<<std::endl;
            break;
        }
        /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

        if(NID_LRBG != 0x332 and NID_LRBG != 0x362)
            float_L_SECTION =  BtoB_distance - D_LRBG + D_Balise_endof_Block;
    }
    if(NID_LRBG == 0x36A) //when the train is between two balises in block 25
    {
        block = C25;
        std::cout<<B25_B250<<std::endl;
        std::cout<<D_LRBG<<std::endl;
        std::cout<<D_Balise_endof_Block<<std::endl;

        float_L_SECTION =  (B25_B250) - (D_LRBG) + (D_Balise_endof_Block);
    }else if (NID_LRBG == 0x382) //when the train is between two balises in block 26
    {
        block = C26;
        float_L_SECTION =  B26_B251 - D_LRBG + D_Balise_endof_Block;
    }

    /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
    std::cout<<"last balise: "<<std::hex<<NID_LRBG << std::hex << " and block = " << std::hex<< block <<std::endl;
    std::cout<< "float_L_SECTION: "<< std::hex << int(float_L_SECTION) <<std::endl;
    /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    /* We assume D_SECTIONTIMERSTOPLOC is equal to L_SECTION. So compute their first and second bytes: */
    //L_SECTION_LSB.at(0) = int(float_L_SECTION) % 256;
    L_SECTION_LSB.push_back(int(float_L_SECTION) % 256); //L_SECTION_LSB[0]

    D_SECTIONTIMERSTOPLOC_LSB.push_back(L_SECTION_LSB.at(0)); //D_SECTIONTIMERSTOPLOC_LSB[0]

    L_SECTION_MSB.push_back(float_L_SECTION / 256); //L_SECTION_MSB[0]
    D_SECTIONTIMERSTOPLOC_MSB.push_back(L_SECTION_MSB.at(0)); //D_SECTIONTIMERSTOPLOC_MSB[0]


    T_SECTIONTIMER.push_back((float_L_SECTION / v[0])+ 1); /* We add 100 ms (each unit is 100 ms) to round up the time and let the train
    leave the block; if we don't add it, the train will stick at the end of block asking MA. In 100 ms, a train can drive 44mm which is not a lot*/
    std::cout<<"v0:"<<int(v[0])<<" lsection: "<<float_L_SECTION<<" timer: "<<int(T_SECTIONTIMER[0])<<std::endl;

    /************************************************* Compute EOA: **************************************************************/
    /*--------------------------debug---------------------------------------------------------------------------------------------------------------*/
    /* print elements of Line: */
    std::set<int>::iterator iter;               //counter of elements of set Lines
    std::cout<<"got these blocks:";
    std::cout<<*Lines.begin();
    for(iter = Lines.begin(); iter!=Lines.end(); ++iter)
    {
        std::cout<<*iter<<" ";
        std::cout.flush();
    }
    std::cout<<"\n";
    /*--------------------------debug---------------------------------------------------------------------------------------------------------------*/

    if (Lines.find(block) == Lines.end())  //if the block doesn't exist in the Line set
        std::cout<<"The block is not in the range"<<std::endl;
    else
        std::cout<<"The block is in the range"<<std::endl;

    /* Find the next block after position of the train: */
    std::set <int>::iterator it = Lines.upper_bound(block);

    /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/
    std::cout<<"______________________"<<*it<<std::endl;
    /*--------------------------debug-----------------------------------------------------------------------------------------------------------------------------------------------------*/

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    /*search to see which is the next occupied block and to send its previous block as EOA: */
    while(it != (Lines.end()))
    {
        if(BlockStatus[*it-29] == 1) //if block was occupied
        {
            EOA = *it-1;
            break;
        }
        it++;
    }
    if(it == Lines.end()) //if blocks are not occupied till the ending block of RBC, or if the train is at the ending block, EOA is the last one
    {
        it--;
        EOA = *it;
    }
    std::cout<<"===========>EOA is: "<<std::hex<<EOA<<std::endl;
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    /* Compute number of sections */
    N_ITER = EOA - block + 1;

    /* Compute lenght of message */
    L_MESSAGE = 49 + 11 * N_ITER;
    MA.push_back(L_MESSAGE); //add L_MESSAGE to MA vector

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    /* Compute T_TRAIN: */
    std::time_t t = std::time(0); //timestamp of RBC
    unsigned char t1 = t %256;  //??should we treat data as they are hex? if not, 256 should be changed to 100
     t = t / 256;
    unsigned char t2 = t % 256;
     t = t / 256;
    unsigned char t3 = t % 256;
    unsigned char t4 = t / 256;
    MA.push_back(t4);
    MA.push_back(t3);
    MA.push_back(t2);
    MA.push_back(t1);
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    MA.push_back(0); //M_ACK = 0

    MA.push_back(NID_LRBG_MSB);
    MA.push_back(NID_LRBG_LSB);

    /**************************************** PACKET 15 **********************************************************************************************************************************/
    MA.push_back(15); //NID_PACKET
    MA.push_back(2); //Q_DIR, validity direction of transmitted data: 2=both
    L_PACKET_15 = (26 + 4 * N_ITER) * 8;
    L_PACKET_15_LSB = L_PACKET_15 % 256;
    L_PACKET_15_MSB = L_PACKET_15 / 256;
    MA.push_back(L_PACKET_15_MSB); //first byte of L_PACKET
    MA.push_back(L_PACKET_15_LSB); //second byte of L_PACKET
    MA.push_back(3); //Q_SCALE= 1mm
    MA.push_back(0); //V_EMA
    MA.push_back(0xFF); //T_EMA; 0xFF = infinity
    MA.push_back(N_ITER);

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    /* Compute length and time out for sections after first section: */
    for(int i=1; i<N_ITER ; i++) //we have calculated everything for the first block, so should calculate variables for the rest of blocks.
    {
        //Length of section for block 25 and 26 should be computed separately
        if (block + i == C25)
            float_L_SECTION = 2 * D_Balise_endof_Block + BalisetoBalise(25, 250);
        else if (block + i == C26)
            float_L_SECTION = 2 * D_Balise_endof_Block + BalisetoBalise(26, 251);
        else
            float_L_SECTION= BalisetoBalise(block+i-0x51, block+i- 0x50); //Length of blocks that are not crossed a node is the same as balise to balise distance
        /* Compute first and second byte of L_SECTION and D_SECTIONTIMERSTOPLOC: */
        L_SECTION_LSB.push_back(int(float_L_SECTION) % 256); //L_SECTION_LSB[i]
        D_SECTIONTIMERSTOPLOC_LSB.push_back(L_SECTION_LSB[i]); //D_SECTIONTIMERSTOPLOC_LSB[i]
        L_SECTION_MSB.push_back(float_L_SECTION / 256);
        D_SECTIONTIMERSTOPLOC_MSB.push_back(L_SECTION_MSB[i]);
        /* Compute T_SECTIONTIMER: */
        T_SECTIONTIMER.push_back(float_L_SECTION / v[(block+i-0x50)-1]); /* This time we don't add 1, so that the expire time of section will be inside the section and
                                                          the train will stop some milimeters before the end of EOA. If we would add 1, the train would
                                                          stop somewhere after EOA which is dangerous.
                                                          - We take v[(block+i-0x50)-1] since v has 29 elements starting from index 0 to 28*/
        std::cout<<"f: "<<float(float_L_SECTION)<<"vi: "<<float(v[(block+i-0x50)-1])<<"ti: "<<float(T_SECTIONTIMER[i])<<"real value: "<<float(float_L_SECTION / v[(block+i-0x50)-1])<<std::endl;
    }
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    /* QSECTIONTIMER means if we use section timer for each section, the answer is yes for all: */
    for(int i = 0; i < N_ITER; i++)
        QSECTIONTIMER.push_back(1);
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    for(int i = 0; i<N_ITER; i++)
    {
        MA.push_back(L_SECTION_MSB[i]);
        MA.push_back(L_SECTION_LSB[i]);
        MA.push_back(QSECTIONTIMER[i]);
        MA.push_back(T_SECTIONTIMER[i]);
        MA.push_back(D_SECTIONTIMERSTOPLOC_MSB[i]);
        MA.push_back(D_SECTIONTIMERSTOPLOC_LSB[i]);
    }

    MA.push_back(L_SECTION_MSB[(N_ITER-1)]); //L_ENDSECTION = L_SECTION_MSB[(N_ITER-1)]
    MA.push_back(L_SECTION_LSB[(N_ITER-1)]); //L_ENDSECTION = L_SECTION_MSB[(N_ITER-1)]

    MA.push_back(0); //Q_SECTIONTIMER
    MA.push_back(0); //T_SECTIONTIMER
    MA.push_back(0); //D_SECTIONTIMERSTOPLOC
    MA.push_back(0); //Q_ENDTIMER
    MA.push_back(0); //T_ENDTIMER
    MA.push_back(0); //D_ENDTIMERSTARTLOC
    MA.push_back(0); //Q_DANGEROINT
    MA.push_back(0); //D_DP
    MA.push_back(0); //V_RELEASEDP
    MA.push_back(0); //Q_OVERLAP
    MA.push_back(0); //D_STARTOL
    MA.push_back(0); //T_TOL
    MA.push_back(0); //D_TOL
    MA.push_back(0); //V_RELEASEOL

    /**************************************** PACKET 80 **********************************************************************************************************************************/
    /**************** FOR THE MOMENT WE DON'T IMPLEMENT MODE VARIABLES AND ASSIGN 0 TO ALL OF THEM ***************************************************************************************/
    MA.push_back(80); //NID_PACKET
    MA.push_back(2); //Q_DIR, validity direction of transmitted data: 2=both
    L_PACKET_80 = (15 + 9 * N_ITER) * 8;
    L_PACKET_80_LSB = L_PACKET_80 % 256;
    L_PACKET_80_MSB = L_PACKET_80 / 256;
    MA.push_back(L_PACKET_80_MSB); //first byte of L_PACKET
    MA.push_back(L_PACKET_80_LSB); //second byte of L_PACKET
    MA.push_back(3); //Q_SCALE: 3 = 1mm
    MA.push_back(0); //D_MAMODE_MSB
    MA.push_back(0); //D_MAMODE_LSB
    MA.push_back(0); //M_MAMODE
    MA.push_back(0); //V_MAMODE
    MA.push_back(0); //L_MAMODE
    MA.push_back(0); //L_ACKMAMODE
    MA.push_back(0); //Q_MAMODE
    MA.push_back(N_ITER);

    for(int K=0; K< N_ITER; K++)
    {
        MA.push_back(0); //D_MAMODE_MSB(K)
        MA.push_back(0); //D_MAMODE_LSB(K)
        MA.push_back(0); //M_MAMODE(K)
        MA.push_back(0); //V_MAMODE(K)
        MA.push_back(0); //L_MAMODE_MSB(K)
        MA.push_back(0); //L_MAMODE_LSB(K)
        MA.push_back(0); //L_ACKMAMODE_MSB(K)
        MA.push_back(0); //L_ACKMAMODE_LSB(K)
        MA.push_back(0); //Q_MAMODE(K)
    }

    return MA;
}

float frame_analysis::BalisetoBalise(int b1, int b2)
{
    switch (b2) {
    case 1:
        if(b1 == 9)
            return B9_B1;
        else if (b2 == 16)
            return B16_B1;
        break;
    case 2:
        return B1_B2;
        break;
    case 3:
        return B2_B3;
        break;
    case 4:
        return B3_B4;
        break;
    case 5:
        return B4_B5;
        break;
    case 6:
        return B5_B6;
        break;
    case 7:
        if(b1 == 6)
            return B6_B7;
        else if(b1 == 13)
            return B13_B7;
        break;
    case 8:
        return B7_B8;
        break;
    case 9:
        return B8_B9;
        break;
    case 10:
        if(b1 == 9)
            return B9_B10;
        else if (b2 == 16)
            return B16_B10;
        break;
    case 11:
        if(b1 == 10)
            return B10_B11;
        else if(b1 == 17)
            return B17_B11;
        else if (b1 == 29)
            return B29_B11;
        break;
    case 12:
        return B11_B12;
        break;
    /* block 13 is inside a node and not a line */
    case 14:
        return B13_B14;
        break;
    case 15:
        return B14_B15;
        break;
    case 16:
        return B15_B16;
        break;
    /* block 17 is inside a node and not a line */
    case 18:
        return B12_B18;
        break;
    case 19:
        return B18_B19;
        break;
    case 20:
        return B19_B20;
        break;
    case 21:
        return B20_B21;
        break;
    case 22:
        if(b1 == 9)
            return B9_B22;
        else if(b1 == 16)
            return B16_B22;
        else if(b1 == 21)
            return B21_B22;
        break;
    case 23:
        return B22_B23;
        break;
    case 24:
        return B23_B24;
        break;
    case 25:
        return B24_B25;
        break;
    case 250:
        return B25_B250;
        break;
    case 26:
        return B250_B26;
        break;
    case 251:
        return B26_B251;
        break;
    case 27:
        return B251_B27;
        break;
    case 28:
        return B27_B28;
        break;
    case 29:
        return B28_B29;
        break;
    default:
        break;
    }
    return 0;
}

std::vector<unsigned char> frame_analysis::TerminationOfACommunicationSession(unsigned char NID_ENGINE)
{
    std::vector<unsigned char> Termination;
    Termination.push_back(156);   //NID_MESSAGE
    Termination.push_back(7);  //L_MESSAGE
    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    /* Compute T_TRAIN: */
    std::time_t t = std::time(0); //timestamp of RBC
    unsigned char t1 = t %256;  //??should we treat data as they are hex? if not, 256 should be changed to 100
     t = t / 256;
    unsigned char t2 = t % 256;
     t = t / 256;
    unsigned char t3 = t % 256;
    unsigned char t4 = t / 256;
    Termination.push_back(t4);
    Termination.push_back(t3);
    Termination.push_back(t2);
    Termination.push_back(t1);
    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
    Termination.push_back(NID_ENGINE);

    /** RBC should of course do some additional activities to disconnet communication by train **/

    return Termination;
}

