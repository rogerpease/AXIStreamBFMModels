#!/bin/sh 

set -e
verilator ../../Verilog/module/AXISimpleStreamFIFO.v --cc --exe TestModule.cpp ../../cpp/AXIMasterStreamBFM.cpp ../../cpp/AXISlaveStreamBFM.cpp  --build  --trace
./obj_dir/VAXISimpleStreamFIFO
exit 0
