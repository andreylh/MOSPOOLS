all: uavrouting
	@echo "BUILT SUCCESSFULLY!"

CPLEX_MAIN_DIR = /opt/ibm/ILOG/CPLEX_Studio128
CPLEXDIR      = $(CPLEX_MAIN_DIR)/cplex
CONCERTDIR    = $(CPLEX_MAIN_DIR)/concert

CCLNFLAGS = -L$(CPLEXDIR)/lib/x86-64_linux/static_pic -DIL_STD -lilocplex -lcplex -L$(CONCERTDIR)/lib/x86-64_linux/static_pic -lconcert -lm -pthread -ldl -m64 
CCINCFLAG = -I$(CPLEXDIR)/include -I$(CONCERTDIR)/include 

uavrouting:
	g++ ./mainCORS_UAVRouting.cpp ./SPOOLStructSmartStorage.cpp ./OptFrame/Scanner++/Scanner.cpp $(CCLNFLAGS) $(CCINCFLAG) --std=c++1z -O3 -o main

greenScheduling:
	g++ ./mainGreen.cpp ./OptFrame/Scanner++/Scanner.cpp $(CCLNFLAGS) $(CCINCFLAG) --std=c++1z -O3 -o main
	
	
clean:
	#make clean -C ./Examples/
	rm -f mainOptFrame
