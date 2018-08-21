all: uavrouting
	@echo "BUILT SUCCESSFULLY!"

CPLEX_MAIN_DIR = /opt/ibm/ILOG/CPLEX_Studio128
CPLEXDIR      = $(CPLEX_MAIN_DIR)/cplex
CONCERTDIR    = $(CPLEX_MAIN_DIR)/concert

CCLNFLAGS = -L$(CPLEXDIR)/lib/x86-64_linux/static_pic -DIL_STD -lilocplex -lcplex -L$(CONCERTDIR)/lib/x86-64_linux/static_pic -lconcert -lm -pthread -ldl -m64 
CCINCFLAG = -I$(CPLEXDIR)/include -I$(CONCERTDIR)/include 
C++FLAGS = --std=c++1z -Ofast
OPTFRAMESCANNER = ./OptFrame/Scanner++/Scanner.cpp

uavrouting:
	g++ ./mainCORS_UAVRouting.cpp $(OPTFRAMESCANNER) $(CCLNFLAGS) $(CCINCFLAG) $(C++FLAGS) -o mainMOSPOOLS

greenScheduling:
	g++ ./mainGreen.cpp $(OPTFRAMESCANNER) $(CCLNFLAGS) $(CCINCFLAG) $(C++FLAGS) -o mainMOSPOOLS

smartstorage:
	g++ ./mainAPEN_SmartStorage.cpp ./SPOOLStructSmartStorage.cpp $(OPTFRAMESCANNER) $(CCLNFLAGS) $(CCINCFLAG) $(C++FLAGS) -o mainMOSPOOLS
	
	
clean:
	make clean
	rm -f mainOptFrame
