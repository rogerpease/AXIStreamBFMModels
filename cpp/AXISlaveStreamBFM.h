#include <vector> 
#include <iostream> 
#include <sstream>
using namespace std; 

class AXISlaveStreamBFM 
{ 

    public :  // Will make private once I'm further down the road. 
        AXISlaveStreamBFM();
        void CaptureCycle();
        void Initialize();
        void ReportPipeStatus(ostringstream & ostream);

        bool DEBUG = true;

        vector<unsigned long> StreamCurrentPacket; 
        vector<vector<unsigned long>> StreamCompletePackets; 



        // Master Stream Ports. TVALID indicates that the master is driving a valid transfer, A transfer takes place when both TVALID and TREADY are asserted.
        unsigned char * S_AXIS_TVALID; // Master Output 
        // TDATA is the primary payload that is used to provide the data that is passing across the interface from the master.
        unsigned long int * S_AXIS_TDATA; // Master Output 
        // TSTRB is the byte qualifier that indicates whether the content of the associated byte of TDATA is processed as a data byte or a position byte.
        unsigned char * S_AXIS_TSTRB; // Master Output 
        // TLAST indicates the boundary of a packet.
        unsigned char * S_AXIS_TLAST; // Master input 
        // TREADY indicates that the slave can accept a transfer in the current cycle.
        unsigned char * S_AXIS_TREADY; // Master input 

        int treadyDropPeriod; // Master input 
        int cycleCount; // Master input 

}; 
