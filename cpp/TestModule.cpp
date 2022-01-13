#include "AXIMasterStreamBFM.h"
#include "AXISlaveStreamBFM.h"
#include <iostream>
#include <memory> 
#include <sstream> 
#include <cassert> 


unsigned char AXIS_TVALID; // Master Output
unsigned long int AXIS_TDATA; // Master Output
unsigned char AXIS_TSTRB; // Master Output
unsigned char AXIS_TLAST; // Master input
unsigned char AXIS_TREADY; // Master input



int main(int argc, char **argv) 
{

   cout << "Building " << endl; 
   AXIMasterStreamBFM AXIMasterStream; 
   AXISlaveStreamBFM AXISlaveStream; 
   cout << "Resetting " << endl; 

   AXIMasterStream.M_AXIS_TVALID = &AXIS_TVALID; 
   AXISlaveStream.S_AXIS_TVALID  = &AXIS_TVALID; 
   AXIMasterStream.M_AXIS_TDATA  = &AXIS_TDATA; 
   AXISlaveStream.S_AXIS_TDATA   = &AXIS_TDATA; 
   AXIMasterStream.M_AXIS_TSTRB  = &AXIS_TSTRB; 
   AXISlaveStream.S_AXIS_TSTRB   = &AXIS_TSTRB; 
   AXIMasterStream.M_AXIS_TLAST  = &AXIS_TLAST; 
   AXISlaveStream.S_AXIS_TLAST   = &AXIS_TLAST; 
   AXIMasterStream.M_AXIS_TREADY = &AXIS_TREADY; 
   AXISlaveStream.S_AXIS_TREADY  = &AXIS_TREADY; 

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

   cout << "Comparing sizes" << endl;    
   assert (streamPacketsGolden.size() == AXISlaveStream.StreamCompletePackets.size()); 
   int packetNum = 0; 
   bool fail = false; 
   for (auto streamPacketGolden: streamPacketsGolden) 
   {
      auto recdPacket = AXISlaveStream.StreamCompletePackets[packetNum]; 
      if (streamPacketGolden.size() != recdPacket.size()) 
      {
        fail = true; 
        cout << "Packet " << packetNum << " size mismatch" << endl; 
      } 
      if (!equal(recdPacket.begin(),recdPacket.end(), streamPacketGolden.begin())) 
      { 
        fail = true; 
        cout << "Packet " << packetNum << " value mismatch" << endl; 
      }
      packetNum++;
   } 

   ostringstream res;
   AXISlaveStream.ReportPipeStatus(res);
   cout << res.str(); 

   if (fail) cout << "FAIL" << endl; else cout << "PASS" << endl; 

   return !fail; 

}
