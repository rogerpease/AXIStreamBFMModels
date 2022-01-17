#include <vector> 
#include <iostream> 
#include "AXIMasterStreamBFM.h" 
using namespace std; 


AXIMasterStreamBFM::AXIMasterStreamBFM(
           unsigned char * S_AXIS_TVALID,
           unsigned int  * S_AXIS_TDATA,
           unsigned char * S_AXIS_TSTRB, // Output 
           unsigned char * S_AXIS_TLAST, // Slave output 
           unsigned char * S_AXIS_TREADY)
{ 
   cycleCount = 0; 
   tvalidDropPeriod = 0; 
   masterName = "Master BFM";
   this->M_AXIS_TVALID = S_AXIS_TVALID;
   this->M_AXIS_TDATA  = S_AXIS_TDATA;
   this->M_AXIS_TSTRB  = S_AXIS_TSTRB; 
   this->M_AXIS_TLAST  = S_AXIS_TLAST; 
   this->M_AXIS_TREADY = S_AXIS_TREADY; 
}   

void AXIMasterStreamBFM::Initialize()
{
   *(this->M_AXIS_TVALID) = 0; // Can't do this before because we've not assigned. 
}
   
void AXIMasterStreamBFM::DriveCycle()
{
  
  static bool lastCycleReady   = false; 
  static bool lastCycleHadData = false; 
  bool thisCycleHasData = false; 

  bool lastCycleMovedData = (*(this->M_AXIS_TVALID) && lastCycleReady);

  if (DEBUG) cout << masterName << " Valid " << (*(this->M_AXIS_TVALID) ? '1' : '0');  
             cout << " Ready " << (lastCycleReady ? '1' : '0')  << endl; 
   

  // New data should always be in StreamCurrentPacket, even if it's impossible to move. 
  if (StreamCurrentPacket.size() == 0)
  { 
    if (StreamCompletePackets.size() != 0)
    {
      StreamCurrentPacket = StreamCompletePackets[0]; 
      StreamCompletePackets.erase(StreamCompletePackets.begin()); 
    } 
  } 

  // Three options here, assuming we have data to move. 
  //   We had data and moved it.
  //   We had data and didn't move it. 
  //   We didn't have data. 


  // assume we don't have data until proven otherwise. 
  thisCycleHasData = false;

  if (((!lastCycleHadData) || (lastCycleMovedData)) && !StreamCurrentPacket.size()) 
  {
     // We either have nothing to move or moved on our last cycle. 
     *(this->M_AXIS_TDATA) = 0; 
     *(this->M_AXIS_TVALID)  = 0; 
     lastCycleHadData = false; 
     if (DEBUG) cout << masterName << "Nothing to transmit" << endl; 
     return; 
  } 
  else if ((lastCycleMovedData) || (!lastCycleHadData))  
  {
      // We moved data on last cycle or we didn't have anything to move. 
      if (StreamCurrentPacket.size()) // If we do have it now. 
      {
        *(this->M_AXIS_TDATA) = StreamCurrentPacket[0]; 
        cout << masterName << " Pushed " << *(this->M_AXIS_TDATA) << endl;
        StreamCurrentPacket.erase(StreamCurrentPacket.begin()); 
        thisCycleHasData = true; 
      }
      if (StreamCurrentPacket.size()) 
        *(this->M_AXIS_TLAST)  = 0; 
      else 
        *(this->M_AXIS_TLAST)  = 1; 
   } 

   // If we had data on the last cycle and didn't move it. 
   if ((lastCycleHadData) && (!lastCycleMovedData)) 
   {  
     thisCycleHasData = true;
   } 
   cout << masterName << " ThisCycleHasData " << ((thisCycleHasData) ? '1' : '0') << endl;
  
   // Drop TVALID every N tvalidDropPeriod cycles (unless TREADY set to 0).
   *(this->M_AXIS_TVALID) = (!thisCycleHasData) ? 0 : (tvalidDropPeriod == 0) ? 1 : (cycleCount % tvalidDropPeriod == 0) ? 0 : 1; 
   lastCycleHadData  = thisCycleHasData;
   lastCycleReady    = *(this->M_AXIS_TREADY); 
   cycleCount ++; 

}  

void AXIMasterStreamBFM::ReportPipeStatus(ostringstream & ostream)
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
