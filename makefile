all: mospools
	@echo "BUILT SUCCESSFULLY!"

CPLEX_MAIN_DIR = /opt/ibm/ILOG/CPLEX_Studio128
CPLEXDIR      = $(CPLEX_MAIN_DIR)/cplex
CONCERTDIR    = $(CPLEX_MAIN_DIR)/concert

CCLNFLAGS = -L$(CPLEXDIR)/lib/x86-64_linux/static_pic -DIL_STD -lcplex -L$(CONCERTDIR)/lib/x86-64_linux/static_pic -lconcert -m64 -lm -pthread
CCINCFLAG = -I$(CPLEXDIR)/include -I$(CONCERTDIR)/include 



mospools:
	g++ ./mainGreen.cpp ./OptFrame/Scanner++/Scanner.cpp $(CCLNFLAGS) $(CCINCFLAG) --std=c++1z -O3 -o main
	#g++ ./mainCORS_UAVRouting.cpp ./SPOOLStructSmartStorage.cpp ./OptFrame/Scanner++/Scanner.cpp $(CCLNFLAGS) $(CCINCFLAG) --std=c++1z -O3 -o main

	
clean:
	#make clean -C ./Examples/
	rm -f mainOptFrame
