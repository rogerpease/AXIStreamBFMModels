#include <vector> 
#include <iostream> 
#include "AXISlaveStreamBFM.h" 
using namespace std; 


AXISlaveStreamBFM::AXISlaveStreamBFM()
{ 
   cycleCount = 0; 
   treadyDropPeriod = 7; 
}   

void AXISlaveStreamBFM::Initialize()
{
   *(this->S_AXIS_TREADY) = 1; // Can't do this before because we've not assigned. 
}
   
void AXISlaveStreamBFM::CaptureCycle()
{
  if (DEBUG) cout << " SLAVE: TValid " << ((*(this->S_AXIS_TVALID) == 1) ? '1' : '0') << "  TREADY " << ((*(this->S_AXIS_TREADY) == 1) ? '1' : '0') << endl; 
  if (*(this->S_AXIS_TVALID) && *(this->S_AXIS_TREADY))
  { 
     unsigned long int data = *(this->S_AXIS_TDATA); 
     if (DEBUG) cout << " Received " << data; 
     StreamCurrentPacket.push_back(data); 
     if (*(this->S_AXIS_TLAST))
     {
       if (DEBUG) cout << " Last of Frame " << data << endl; 
       StreamCompletePackets.push_back(StreamCurrentPacket); 
       StreamCurrentPacket.clear(); 
     } 
     if (DEBUG) cout << endl;
   } 
  
   // Drop TREADY every N treadyDropPeriod cycles (unless TREADY set to 0).
   *(this->S_AXIS_TREADY) = (treadyDropPeriod == 0) ? 1 : (cycleCount % treadyDropPeriod == 0) ? 0 : 1; 
   cycleCount ++; 
}  

void AXISlaveStreamBFM::ReportPipeStatus(ostringstream & ostream)
{
     ostream << "Current: ";
     for (auto a: this->StreamCurrentPacket)
        ostream << hex << a << dec << " ";
     ostream << endl;
     ostream << "Complete: ";
     for (auto a: this->StreamCompletePackets)
     {
       for (auto b: a)
         ostream << hex << b << dec << " ";
       ostream << endl;
     }

}
