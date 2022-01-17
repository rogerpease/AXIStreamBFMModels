#include "AXIMasterStreamBFM.h"
#include "AXISlaveStreamBFM.h"
#include <iostream>
#include <memory> 
#include <sstream> 
#include <cassert> 


unsigned char AXIS_TVALID; // Master Output
unsigned int AXIS_TDATA; // Master Output
unsigned char AXIS_TSTRB; // Master Output
unsigned char AXIS_TLAST; // Master input
unsigned char AXIS_TREADY; // Master input



int main(int argc, char **argv) 
{

   cout << "Building " << endl; 
   AXIMasterStreamBFM AXIMasterStream( &AXIS_TVALID, &AXIS_TDATA, &AXIS_TSTRB, &AXIS_TLAST, &AXIS_TREADY); 
   AXISlaveStreamBFM AXISlaveStream( &AXIS_TVALID, &AXIS_TDATA, &AXIS_TSTRB, &AXIS_TLAST, &AXIS_TREADY); 
   cout << "Resetting " << endl; 

   vector<vector<unsigned long int>> streamPacketsGolden;  
   streamPacketsGolden.push_back({0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08});  
   streamPacketsGolden.push_back({0xDECADE01,0xDECADE02,0xDECADE03,0xDECADE04,0xDECADE05,0xDECADE06,0xDECADE07,0xDECADE08});  
   streamPacketsGolden.push_back({0xDEFACED1,0xDEFACED2,0xDEFACED3,0xDEFACED4,0xDEFACED5,0xDEFACED6,0xDEFACED7,0xDEFACED8});  
   streamPacketsGolden.push_back({0xDEBA7E01,0xDEBA7E02,0xDEBA7E03,0xDEBA7E04,0xDEBA7E05,0xDEBA7E06,0xDBA7EE07,0xDEBA7E08,
                                  0xDEBA7E09,0xDEBA7E0A,0xDEBA7E0B,0xDEBA7E0C,0xDEBA7E0D,0xDEBA7E0E,0xDBA7EE0F}); 

   for (auto a: streamPacketsGolden) 
      AXIMasterStream.StreamCompletePackets.push_back(a); 

   AXIMasterStream.Initialize();
   AXISlaveStream.Initialize();

   for (int i = 0; i < 200; i++)
   {
     AXIMasterStream.DriveCycle();
     AXISlaveStream.CaptureCycle(); 
   } 
   
   stringstream transcript;
   assert(AXISlaveStream.CompareContents(streamPacketsGolden,transcript)); 
   assert(!AXISlaveStream.CompareContents({{0x12,0x34}},transcript)); 

   ostringstream res;
   AXISlaveStream.ReportPipeStatus(res);
   cout << res.str(); 

   cout << "PASS" << endl; 

   return 0; 

}
