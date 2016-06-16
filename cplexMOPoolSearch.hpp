/*
 * OptimalLinearRegression.hpp
 *
 *  Created on: 26/10/2014
 *      Author: vitor
 */

#ifndef CPLEXMOPOOLSEARCH_HPP_
#define CPLEXMOPOOLSEARCH_HPP_

#include <ilcplex/ilocplex.h>
#include <ilcplex/cplexx.h>
#include <ilcplex/ilocplexi.h>

#include <iterator>
#include <algorithm>
#include <numeric>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>      // std::ifstream
#include <sstream>
#include "OptFrame/Util/MultiObjectiveMetrics2.hpp"
#include "OptFrame/RandGen.hpp"
#include "OptFrame/Timer.hpp"
#include "OptFrame/Util/printable.h"

#include <vector>

ILOSTLBEGIN

using namespace std;

using namespace optframe;

struct MIPStartSolution
{
	vector<double> objValues;
	string filename;
	MIPStartSolution(vector<double> _obj, string _filename) :
			objValues(_obj), filename(_filename)
	{
	}
};

char* execCommand(const char* command)
{

	FILE* fp;
	char* line = NULL;
// Following initialization is equivalent to char* result = ""; and just
// initializes result to an empty string, only it works with
// -Werror=write-strings and is so much less clear.
	char* result = (char*) calloc(1, 1);
	size_t len = 0;

	fflush(NULL);
	fp = popen(command, "r");
	if (fp == NULL)
	{
		printf("Cannot execute command:\n%s\n", command);
		return NULL;
	}

	while (getline(&line, &len, fp) != -1)
	{
		// +1 below to allow room for null terminator.
		result = (char*) realloc(result, strlen(result) + strlen(line) + 1);
		// +1 below so we copy the final null terminator.
		strncpy(result + strlen(result), line, strlen(line) + 1);
		free(line);
		line = NULL;
	}

	fflush(fp);
	if (pclose(fp) != 0)
	{
		perror("Cannot close stream.\n");
	}

	return result;
}

class cplexMOPoolSearch
{

public:
	RandGen& rg;

	cplexMOPoolSearch(RandGen& _rg) :
			rg(_rg)

	{

	}

	~cplexMOPoolSearch()
	{

	}

//	void combineAll(const vector<vector<double> > &allVecs, size_t vecIndex, int aux, int auxComb, vector<vector<double> > &comb)
//	{
//		int nVec = allVecs.size();
//		if (vecIndex >= allVecs.size())
//		{
//			if (aux == 0)
//				cout << "\n";
//
//
//			return;
//		}
//
//		for (size_t i = 0; i < allVecs[vecIndex].size(); i++)
//		{
//			cout << allVecs[vecIndex][i] << "\t";
//			comb[auxComb].push_back(allVecs[vecIndex][i]);
//
//			combineAll(allVecs, vecIndex + 1, i, auxComb, comb);
//		}
//	}

//	void analyzeParetoFronts(string outputPF1, int sizePF1, string outputPF2, int sizePF2)
//	{
//		int nObj = 3;
//		UnionNDSets2 uND(nObj);
//		//===================================
//		// TEST PARETO
//
//		vector<vector<double> > PF1 = uND.unionSets(outputPF1.c_str(), sizePF1);
//		vector<vector<double> > PF2 = uND.unionSets(outputPF2.c_str(), sizePF2);
//		vector<vector<double> > ref = uND.unionSets(PF1, PF2);
//		cout << PF1.size() << endl;
//		cout << PF2.size() << endl;
//		cout << ref.size() << endl;
//		int card = uND.cardinalite(PF1, ref);
//		double sCToRef = uND.setCoverage(PF1, ref);
//		double sCFromRef = uND.setCoverage(ref, PF1);
//		vector<double> utopicSol;
//		utopicSol.push_back(-10000);
//		utopicSol.push_back(0);
//		utopicSol.push_back(0);
//		//Delta Metric and Hipervolume need to verify min
//		cout << "Cardinalite = " << uND.cardinalite(PF1, ref) << endl;
//		cout << "Cardinalite = " << uND.cardinalite(PF2, ref) << endl;
//		cout << "uND.setCoverage(PF1, ref)  = " << uND.setCoverage(PF1, ref) << endl;
//		cout << "uND.setCoverage(PF2, ref)  = " << uND.setCoverage(PF2, ref) << endl;
//		cout << "uND.setCoverage(PF2, PF1)  = " << uND.setCoverage(PF2, PF1) << endl;
//		cout << "uND.setCoverage(PF1, PF2)  = " << uND.setCoverage(PF1, PF2) << endl;
//		cout << "delta  = " << uND.deltaMetric(ref, utopicSol) << endl;
//		cout << "delta  = " << uND.deltaMetric(PF1, utopicSol) << endl;
//		cout << "delta  = " << uND.deltaMetric(PF2, utopicSol) << endl;
//		cout << "hv  = " << hipervolume(ref) << endl;
//		cout << "hv  = " << hipervolume(PF1) << endl;
//		cout << "hv  = " << hipervolume(PF2) << endl;
//
//		cout << "analises done with sucess!" << endl;
//		//============
//	}

//	void printVectorOfVector(vector<vector<double> > vToPrint)
//	{
//		int nVec = vToPrint.size();
//		for (int v = 0; v < nVec; v++)
//		{
//			for (int s = 0; s < vToPrint[v].size(); s++)
//			{
//				cout << vToPrint[v][s] << "\t";
//			}
//			cout << endl;
//		}
//	}

	vector<double> calcMeanDesv(vector<double> v)
	{
		double sum = accumulate(v.begin(), v.end(), 0.0);
		double mean = sum / v.size();

		double sq_sum = inner_product(v.begin(), v.end(), v.begin(), 0.0);
		double stdev = sqrt(sq_sum / v.size() - mean * mean);

		vector<double> result;
		result.push_back(sum);
		result.push_back(stdev);
		return result;
	}

//	vector<int> findLPNumberVariables(IloNumVarArray& var)
//	{
//		vector<int> variablesNumber;
//
////		cout << var << endl;
//		//objCost[-inf..inf] , objWearTear[-inf..inf] , objMaxLoad[-inf..inf]
//		// energySelling(1),energyBuying(1)
//		// yCharge(C1,1,1), yCharge(C1,2,1), ..., yCharge(C3,Cycle,1), ..., yDischarge(C3,Cyle,1)
//		// energySelling(..),energyBuying(..)
//		// yCharge(....) ... yDischarge(...)
//		// BaterryRate(C1,1), BaterryRate(C2,1), ...,  yBaterryRate(pev,Interval)
//		// energySellingActive(1), energySellingActive(2), ..., energyBuyingActive(1), energyBuyingActive(2),...
//		// totalChargingDischargingPayed
//		//						for (int lpVar = 0; lpVar < var.getSize(); lpVar++)
//		//						{
//		//							string varName = var[lpVar].getName();
//		//							cout << varName.c_str() << endl;
//		//							int variableFound = varName.find("yCharge(C1");
//
//		cout << "Finding LP number of variable..." << endl;
//		int counterNumberCycles = 0;
//		int countPEVS = 0;
//		int countIntervals = 0;
//		int variableFound = 0;
//		string varName;
//		while (variableFound == 0)
//		{
//			counterNumberCycles++;
//			varName = var[5 + counterNumberCycles].getName();
//			variableFound = varName.find("yCharge(C1");
//		}
//		variableFound = 0;
//		while (variableFound == 0)
//		{
//			countPEVS++;
//			varName = var[5 + countPEVS].getName();
//			variableFound = varName.find("yCharge");
//		}
//
//		for (int lpVar = 5; lpVar < var.getSize(); lpVar++)
//		{
//			varName = var[lpVar].getName();
//			variableFound = varName.find("yBaterryRate(C1,2");
//
//			while (variableFound == 0)
//			{
//				countIntervals++;
//				varName = var[lpVar + countIntervals].getName();
//				variableFound = varName.find("yBaterryRate(C1");
//			}
//
//			if (countIntervals > 0)
//			{
//				// first constraints was not counted, started cycle 2
//				countIntervals = countIntervals + 1;
//				lpVar = var.getSize();
//			}
//		}
//		countPEVS /= counterNumberCycles;
////		cout << counterNumberCycles << endl;
////		cout << countPEVS << endl;
////		cout << countIntervals << endl;
////		getchar();
//		variablesNumber.push_back(counterNumberCycles);
//		variablesNumber.push_back(countPEVS);
//		variablesNumber.push_back(countIntervals);
//
//		return variablesNumber;
//	}

	void exec(string filename, bool mipStart, int nIntervalsCoef, vector<vector<double> > vMILPCoefs, int tLim, int nOptObj, int nCriteria)
	{

		cout << "Exec cplex MO Matheuristic Pool Sarch." << endl;

		cout << "Number of objectives functions: " << nOptObj << endl;
		UnionNDSets2 uND(nOptObj);

		int nMILPProblems = vMILPCoefs.size();
		cout << "nMILPProblems = " << nMILPProblems << endl;

		vector<vector<double> > Pop;

		IloEnv env;
		try
		{
			Timer tTotal;
			IloCplex cplex(env);
			IloModel model(env);
			IloObjective obj;
			IloNumVarArray var(env);
			IloRangeArray rng(env);

			string modelLPAdress = "./LP/" + filename + ".lp";
			cout << "solving: " << modelLPAdress << endl;

			cplex.importModel(model, modelLPAdress.c_str(), obj, var, rng);

			// ========= Finding LP number of variable ==============
//						vector<int> numberVariables = findLPNumberVariables(var);

//						getchar();
			int mipCounter = 0;
			vector<MIPStartSolution> poolMIPStart;
			for (int milpProblems = 0; milpProblems < nMILPProblems; milpProblems++)
			{
				cout << "=====================================\n";
				cout << "Creating MILP model " << milpProblems << "/" << nMILPProblems << " with: \n";
				for (int o = 0; o < nOptObj; o++)
					cout << "lambda(" << o + 1 << "): " << vMILPCoefs[milpProblems][o] << "\t";
				cout << "\n";

				for (int o = 0; o < nOptObj; o++)
					obj.setLinearCoef(var[o], vMILPCoefs[milpProblems][o]);

				cout << obj << endl;
//							getchar();

				cplex.extract(model);
				cplex.setParam(cplex.TiLim, tLim);

				cplex.readMIPStarts("tempMIPStart/mst44.mst");
				// =====================================================
				//Read the file with the best MIP start
				if (mipCounter > 0)
				{

					int totalNMIPStartSolutions = poolMIPStart.size();
					cout << "Pool of MIP start solution has: " << totalNMIPStartSolutions << " possibilities" << endl;

					double bestFO = 100000000000000;
					int bestMIPStartIndex = -1;
					//Selecting best MIP start for the current weights
					for (int mipS = 0; mipS < totalNMIPStartSolutions; mipS++)
					{
						double currentMILPObj = 0;
						for (int o = 0; o < nOptObj; o++)
							currentMILPObj += vMILPCoefs[milpProblems][o] * poolMIPStart[mipS].objValues[o];

						if (currentMILPObj < bestFO)
						{
							bestFO = currentMILPObj;
							bestMIPStartIndex = mipS;
						}
					}
					cout << "best MIPStart solution is: " << bestFO << "\t index:" << bestMIPStartIndex << endl << endl;
					cplex.readMIPStart(poolMIPStart[bestMIPStartIndex].filename.c_str());
				}
				// =====================================================

				IloBool solveBool = cplex.solve();

				int nCplexPoolOfSolutions = cplex.getSolnPoolNsolns();

				if (!solveBool)
				{
//								env.error() << "Failed to optimize LP" << endl;
//								throw(-1);
					cout << "=====================================\n";
					cout << "Any solution was found! The following parameters were used: \n";
					for (int o = 0; o < nOptObj; o++)
						cout << "lambda(" << o + 1 << "): " << vMILPCoefs[milpProblems][o] << "\t";
					cout << "\n";
					cout << "\t cplex.TiLim: " << tLim << "\n";
					cout << "=====================================\n";
				}
				else
				{
					cout << "=====================================\n";
					cout << "Problem SOLVED!! \n";
					cout << nCplexPoolOfSolutions << " solutions were obtained. \n";
					cout << "=====================================\n\n";
				}

				IloNumArray vals(env);

				if (nCplexPoolOfSolutions > 0)
					for (int nS = 0; nS < nCplexPoolOfSolutions; nS++)
					{
						cout << "=====================================\n";
						cout << "Extracting solution: " << nS + 1 << "/" << nCplexPoolOfSolutions << endl;

						cplex.getValues(vals, var, nS);
						//cout<<cplex.getRangeFilterLowerBound(1);
//cout<<cplex.getRangeFilterLowerBound(0);
//									cout << cplex.getBestObjValue() << endl;
//									cout<<cplex.getObjValue(nS)<<endl; //gets solution obj
//									cout<<cplex.getStatus()<<endl;

//(bestinteger - bestobjective) / (1e-10 + |bestobjective|)

//									getchar();

//											cout << var << endl;
//											getchar();
						vector<double> finalOF;
						for (int o = 0; o < nOptObj; o++)
						{
							finalOF.push_back(vals[o]);
							cout << "obj(" << o + 1 << "): " << finalOF[o] << "\t";
						}
//						cout << "obj(" << 7 << "): " << finalOF[7] << "\t"; //print excess auxiliary variable
						cout << "\n";

						Pop.push_back(finalOF);

						//==============================================
						// Exaustive writing of MST solutions in a file
						if (mipStart)
						{
							cout << "Writing MST solution...." << endl;
							stringstream mstFilename;
							mstFilename << "./tempMIPStart/mst" << mipCounter << ".mst";
//							cplex.writeMIPStarts(mstFilename.str().c_str(), nS, nCplexPoolOfSolutions);
							cplex.writeMIPStart(mstFilename.str().c_str(), nS);
							MIPStartSolution mipStartSol(finalOF, mstFilename.str());
							poolMIPStart.push_back(mipStartSol);
							mipCounter++;
							cout << "MST Saved in file with sucess!" << endl;

							//(WriteLevel, CPX_PARAM_WRITELEVEL) //0 to 4
							//CPX_WRITELEVEL_ALLVARS, CPX_WRITELEVEL_DISCRETEVARS, CPX_WRITELEVEL_NONZEROVARS,CPX_WRITELEVEL_NONZERODISCRETEVARS
						}
						//==============================================
						cout << "Solution: " << nS + 1 << "/" << nCplexPoolOfSolutions << " has been extracted with success and added to the population" << endl;
						cout << "=====================================\n";

					}

				cout << "=====================================\n\n";

			}

			cout << "total time spent: " << tTotal.now() << endl;
			cout << "Obtained population has size :" << Pop.size() << endl;

			if (Pop.size() > 0)
			{
				cout << "Printing obtained population of solutions with size :" << Pop.size() << endl;
				cout << Pop << endl;
				vector<vector<double> > paretoSET = uND.createParetoSet(Pop);
				double nParetoInd = paretoSET.size();

				cout << "Printing Pareto Front of size:" << paretoSET.size() << endl;
				cout << paretoSET << endl;
				vector<vector<vector<double> > > vParetoSet;
				vParetoSet.push_back(paretoSET);

				stringstream ss;
				ss << "./ResultadosFronteiras/" << filename << "NExec" << nMILPProblems << "TLim" << tLim; // << "-bestMIPStart";
				cout << "Writing PF at file " << ss.str() << "..." << endl;
				FILE* fFronteiraPareto = fopen(ss.str().c_str(), "w");
				for (int nS = 0; nS < nParetoInd; nS++)
				{
					for (int nE = 0; nE < nOptObj; nE++)
					{
						fprintf(fFronteiraPareto, "%.5f \t ", paretoSET[nS][nE]);
					}
					fprintf(fFronteiraPareto, "\n");
				}
				fprintf(fFronteiraPareto, "%.5f \n ", tTotal.now());

				fclose(fFronteiraPareto);
			}
			else
			{
				cout << "Any solution was obtained among the: " << nMILPProblems << "  MILP optimizations with different weighted-sum!" << endl;
			}

			//cout << vals[vals.getSize()] << endl;
			try
			{ // basis may not exist
				IloCplex::BasisStatusArray cstat(env);
				cplex.getBasisStatuses(cstat, var);
				env.out() << "Basis statuses  = " << cstat << endl;
			} catch (...)
			{
			}

			//env.out() << "Maximum bound violation = " << cplex.getQuality(IloCplex::MaxPrimalInfeas) << endl;
		} catch (IloException& e)
		{
			cerr << "Concert exception caught: " << e << endl;
		} catch (...)
		{
			cerr << "Unknown exception caught" << endl;
		}

		env.end();
		cout << "cplex MO Pool Search finished com sucesso!" << endl;
	}

// 2

};

#endif /* CPLEXMOPOOLSEARCH_HPP_ */
