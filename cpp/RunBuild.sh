#!/bin/sh 

set -e 
g++ TestModule.cpp AXISlaveStreamBFM.cpp AXIMasterStreamBFM.cpp -o TestTogether
g++ -c AXISlaveStreamBFM.cpp -o AXISlaveStreamBFM.o
g++ -c AXIMasterStreamBFM.cpp -o AXIMasterStreamBFM.o
ar rvs libAXIStreamBFM.a AXIMasterStreamBFM.o AXISlaveStreamBFM.o 

exit 0
