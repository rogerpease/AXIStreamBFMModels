#include "VAXISimpleStreamFIFO.h"
#include "../../cpp/AXIMasterStreamBFM.h"
#include "../../cpp/AXISlaveStreamBFM.h"
#include <iostream>
#include <memory>
#include <sstream>
#include <functional>
#include <verilated_vcd_c.h>



class AXISimpleStreamFIFOWrapper : public VAXISimpleStreamFIFO
{

  public:

  bool debug = true;
  int tickCount; 

  AXIMasterStreamBFM * MasterStreamBFM;
  AXISlaveStreamBFM * SlaveStreamBFM; 
  VerilatedVcdC	*m_trace;
  bool enableTrace; 

  AXISimpleStreamFIFOWrapper()
  {
    const std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};
    VAXISimpleStreamFIFO{contextp.get(), "VAXISimpleStream"};
    MasterStreamBFM = new AXIMasterStreamBFM(&dataIn_tvalid,&dataIn_tdata, &dataIn_tstrb, &dataIn_tlast, &dataIn_tready);       
    SlaveStreamBFM  = new AXISlaveStreamBFM(&dataOut_tvalid,&dataOut_tdata,&dataOut_tstrb,&dataOut_tlast,&dataOut_tready); 

    tickCount = 0; 
    enableTrace = false; 

    m_trace = nullptr;
 
  }

  virtual void opentrace(const char *vcdname) {
    if (!m_trace) {
      m_trace = new VerilatedVcdC();
      this->trace(m_trace,99); 
      m_trace->open(vcdname);
    }
    enableTrace = true; 
  }

   // Close a trace file
   virtual void	closetrace(void) {
     if (m_trace) {
       m_trace->close();
       m_trace = NULL;
     }
   }

  void SetReset(int val) 
  { 
     dataIn_treset = val; dataOut_treset = val; 

  } 
  void ToggleClock() { 
     tickCount ++; 
     dataIn_tclk = 1; dataOut_tclk = 1; this->eval(); 
     if(enableTrace) m_trace->dump(10*tickCount-2);

     tickCount ++; 
     dataIn_tclk = 0; dataOut_tclk = 0; this->eval(); 
     if(enableTrace) m_trace->dump(10*tickCount-2);
  } 

  void Toggle() 
  {
     MasterStreamBFM->DriveCycle();
     SlaveStreamBFM->CaptureCycle();
     ToggleClock(); 
  } 

};
  

int main()
{
  Verilated::traceEverOn(true);
 
  AXISimpleStreamFIFOWrapper * obj     = new AXISimpleStreamFIFOWrapper();  
  
  cout << "Open Trace" << endl; 
  obj->opentrace("Roger1.vcd"); 
  cout << "End Trace" << endl; 

  obj->SetReset(1); 
  obj->ToggleClock(); 
  obj->SetReset(0); 
  obj->ToggleClock(); 

  obj->ToggleClock(); 

  vector<vector<unsigned long int>> goldenPackets = { 
                {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08},
                {0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18},
                {0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28},
                {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x039,0x40,0x12345,0x41,0x42},
                {0x41},
                {0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D},
                {0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x069,0x70,0x71,0x72,0x73}}; 

  for (auto a : goldenPackets)
    obj->MasterStreamBFM->StreamCompletePackets.push_back(a);  

  for (int i = 10; i<200;i++) { obj->Toggle(); } 

  obj->closetrace(); 

  stringstream transcript; 
  if (!obj->SlaveStreamBFM->CompareContents(goldenPackets, transcript ))
    cout << transcript.str(); 
  
  cout << "PASS" << endl; 
  
  return 0; 

}
