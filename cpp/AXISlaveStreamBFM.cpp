#include <vector> 
#include <iostream> 
#include "AXISlaveStreamBFM.h" 
using namespace std; 


AXISlaveStreamBFM::AXISlaveStreamBFM(
                    unsigned char * M_AXIS_TVALID,
                    unsigned int * M_AXIS_TDATA,
                    unsigned char * M_AXIS_TSTRB, // Output   
                    unsigned char * M_AXIS_TLAST, // Slave output  
                    unsigned char * M_AXIS_TREADY                                                                                                                                                                 )                                
{ 
   cycleCount = 0; 
   treadyDropPeriod = 7; 
   slaveName = "SLAVE BFM "; 
   this->S_AXIS_TVALID = M_AXIS_TVALID;
   this->S_AXIS_TDATA  = M_AXIS_TDATA;
   this->S_AXIS_TSTRB  = M_AXIS_TSTRB;
   this->S_AXIS_TLAST  = M_AXIS_TLAST;
   this->S_AXIS_TREADY = M_AXIS_TREADY;      
   this->Initialize(); 
}   

void AXISlaveStreamBFM::Initialize()
{
   *(this->S_AXIS_TREADY) = 1; // Can't do this before because we've not assigned. 
}
   
void AXISlaveStreamBFM::CaptureCycle()
{
  if (DEBUG) cout << slaveName << " TValid " << ((*(this->S_AXIS_TVALID) == 1) ? '1' : '0') << "  TREADY " << ((*(this->S_AXIS_TREADY) == 1) ? '1' : '0') << 
                                  " TDATA "  << *(this->S_AXIS_TDATA) << " TLAST " << (*(this->S_AXIS_TLAST) ? '1' : '0') << endl ; 


  if (*(this->S_AXIS_TVALID) && *(this->S_AXIS_TREADY))
  { 
     unsigned long int data = *(this->S_AXIS_TDATA); 
     if (DEBUG) cout << slaveName << " Captured " << data; 
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


bool AXISlaveStreamBFM::CompareContents(vector<vector<unsigned long int>> expected, stringstream& transcript  )
{
  bool result = true; 
  if (expected.size() != this->StreamCompletePackets.size())
  {
    transcript << "Expected " << expected.size() << " Actual " << this->StreamCompletePackets.size() << endl; 
    result = false; 
  } 
  for (int i = 0; i < max(expected.size(), this->StreamCompletePackets.size()); i++) 
  {

    bool expectedVectorValid = (expected.size() > i); 
    int expectedVectorSize = (expectedVectorValid) ? expected[i].size() : 0; 

    bool actualVectorValid = (this->StreamCompletePackets.size() > i); 
    int actualVectorSize   = (actualVectorValid)   ? this->StreamCompletePackets[i].size()   : 0; 

    for (int j = 0; j < max(expectedVectorSize, actualVectorSize); j++) 
    {
      bool expectedValueValid = ((expectedVectorValid) && (expected[i].size() > j)) ? true : false;  
      bool actualValueValid   = ((actualVectorValid) && (this->StreamCompletePackets[i].size() > j)) ? true : false;  
      int expectedValue = (expectedValueValid) ? expected[i][j] : 0; 
      int actualValue   = (actualValueValid)   ? this->StreamCompletePackets[i][j] : 0; 
      if (expectedValueValid && actualValueValid && (actualValue == expectedValue)) 
      {
      } 
      else 
      { 
        transcript << " Vector  " << i << " " << j << " "; 
        if (expectedValueValid) transcript << " " << expectedValue << " ";  else transcript << " ******** "; 
        if (actualValueValid)   transcript << " " << actualValue << " ";  else transcript << " ******** "; 
        transcript << endl; 
        result = false; 
      } 
    }  
  } 
  return result; 
}

