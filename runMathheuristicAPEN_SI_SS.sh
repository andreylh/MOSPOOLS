#!/bin/bash

PS=" 
APEN_SI/REED_1House_1PEVs_3UAVs_120FH
APEN_SI/REED_1House_1PEVs_5UAVs_120FH
APEN_SI/REED_1House_2PEVs_3UAVs_120FH
APEN_SI/REED_1House_2PEVs_5UAVs_120FH
APEN_SI/ResidencialArea_A_20PEVs_20UAVs_120FH
APEN_SI/ResidencialArea_A_20PEVs_50UAVs_120FH
APEN_SI/ResidencialArea_A_5PEVs_20UAVs_120FH
APEN_SI/ResidencialArea_A_5PEVs_50UAVs_120FH
"

TIMELIMIT="10 30 60"

for timeLimit in $TIMELIMIT
  do
	for probl in $PS 
	do 
   		echo "Resolvendo o problema $probl with time limit $timeLimit and mip start true"
  		./main $probl $timeLimit 1 -1 -1
	  done
done


