//
// Simple FIFO. 
//
module AXISimpleStreamFIFO
#(
parameter DATAINWIDTH = 32, // Must be Multiple of 8  
parameter FIFODEPTH   = 32  // Must be 2**n 
)
(
    input       [(DATAINWIDTH-1):0] dataIn_tdata, 
    input       [(DATAINWIDTH/8-1):0] dataIn_tstrb,
    input       dataIn_tvalid,
    output reg  dataIn_tready,
    input       dataIn_tlast,
    input       dataIn_tclk,
    input       dataIn_treset,

    output  reg [(DATAINWIDTH-1):0]   dataOut_tdata, 
    output  reg [(DATAINWIDTH/8-1):0] dataOut_tstrb, 
    output  reg dataOut_tvalid,
    input       dataOut_tready,
    output  reg dataOut_tlast,
    input       dataOut_tclk,
    input       dataOut_treset

);

    reg [DATAINWIDTH:0] FIFORegisters [FIFODEPTH-1:0]; 
    reg [$clog2(FIFODEPTH)-1:0] FifoStart; 
    reg [$clog2(FIFODEPTH)-1:0] FifoEnd; 
    
    wire [DATAINWIDTH:0] dataIn; 
    assign dataIn = {dataIn_tlast,dataIn_tdata[(DATAINWIDTH-1):0]}; 
  
    reg dataIn_tvalid_q; 
    
    always @(posedge dataIn_tclk)
    begin
      if (dataIn_treset)
        begin
          FifoEnd = 0; 
          dataIn_tvalid_q <= 0; 
        end 
      else 
        begin
//          $display ("AXISimpleStreamFIFO Slave-side FIFO End ", FifoEnd, 
//                  " FifoRegisters[FifoEnd-1] ",FIFORegisters[FifoEnd-1],
//                " dataIn_tvalid ",dataIn_tvalid_q, " dataIn_tready ", dataIn_tready, " dataIn ", dataIn);  
          if (dataIn_tvalid_q && dataIn_tready) // tready from the last cycle 
          begin 
      /* verilator lint_off WIDTH */ 
            FIFORegisters[FifoEnd] = dataIn; 
 //           $display("AXISimpleSteramFIFO Captured ", dataIn," to FifoReg ",FifoEnd); 
            FifoEnd = FifoEnd + 1; 
      /* verilator lint_on WIDTH */ 
          end  
          dataIn_tvalid_q <= dataIn_tvalid; 
       end 
    end 

    always @(posedge dataIn_tclk)
    begin 
      if (FifoStart != FifoEnd+1) 
      begin 
        dataIn_tready = 1;
      end
      else
      begin 
        dataIn_tready = 0;
      end 
    end 

    wire dataToTransmit;
    wire lastCycleValid;
    assign dataToTransmit = (FifoStart != FifoEnd);
    assign lastCycleValid = dataOut_tvalid && dataOut_tready; 
  
    always @(posedge dataOut_tclk)
    begin 
      if (dataOut_treset)
        begin
          FifoStart = 0; 
          dataOut_tlast = 0;
      /* verilator lint_off WIDTH */ 
      /* verilator lint_on WIDTH */ 
          dataOut_tvalid = 0;
        end 
      else 
        begin
      /* verilator lint_off WIDTH */ 


           dataOut_tlast = FIFORegisters[FifoStart][DATAINWIDTH] && dataToTransmit;   
           dataOut_tdata = FIFORegisters[FifoStart][DATAINWIDTH-1:0];  

           if (lastCycleValid)
           begin 
      /* verilator lint_on WIDTH */ 
              FifoStart     = FifoStart + 1; 
           end 
           dataOut_tvalid = dataOut_tvalid_d;
        end 
//        $display("AXISimpleStreamFifo Master-side FifoStart ",FifoStart," Value ",FIFORegisters[FifoStart],
//                 " dataOut_tdata ", dataOut_tdata, " dataOut_tlast ", dataOut_tlast); 
    end 

    reg dataOut_tvalid_d;

    always_comb
    begin 
      if (FifoStart != FifoEnd) 
      begin 
        dataOut_tvalid_d = 1;
      end
      else
      begin 
        dataOut_tvalid_d = 0;
      end 
    end 


endmodule 
