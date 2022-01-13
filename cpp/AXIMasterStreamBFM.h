#include <vector> 
#include <iostream> 
#include <sstream>
using namespace std; 

class AXIMasterStreamBFM
{ 

    public :  // Will make private once I'm further down the road. 
        AXIMasterStreamBFM();
        void DriveCycle();
        void Initialize();
        void ReportPipeStatus(ostringstream & ostream);

        bool DEBUG = false;

        // Master Stream Ports. TVALID indicates that the master is driving a valid transfer, A transfer takes place when both TVALID and TREADY are asserted.
        unsigned char * M_AXIS_TVALID; 
        // TDATA is the primary payload that is used to provide the data that is passing across the interface from the master.
        unsigned long int * M_AXIS_TDATA;
        // TSTRB is the byte qualifier that indicates whether the content of the associated byte of TDATA is processed as a data byte or a position byte.
        unsigned char * M_AXIS_TSTRB; // Output 

        // TLAST indicates the boundary of a packet.
        unsigned char * M_AXIS_TLAST; // Slave output 
        // TREADY indicates that the slave can accept a transfer in the current cycle.
        unsigned char * M_AXIS_TREADY; // Slave input 
  
        vector<unsigned long int>          StreamCurrentPacket;  
        vector<vector<unsigned long int>>  StreamCompletePackets;  
	int cycleCount; 
	int tvalidDropPeriod; 

}; 
