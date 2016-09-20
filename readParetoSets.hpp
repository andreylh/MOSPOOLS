#ifndef READPARETOSETS_HPP_
#define READPARETOSETS_HPP_

#include "OptFrame/MultiObjSearch.hpp"
#include "OptFrame/RandGen.hpp"

#include <unistd.h>

using namespace std;
using namespace optframe;

class readParetoSets
{
public:
	MOMETRICS<int> moMetrics;
	int nOptObj;
public:

	readParetoSets(int _nOptObj, MOMETRICS<int> _moMetrics) :
			nOptObj(_nOptObj), moMetrics(_moMetrics)
	{

	}

	~readParetoSets()
	{

	}

	void exec(vector<string> vInputModel, vector<double> utopicSol, vector<double> referencePointsHV)
	{
		cout << "Exec Math read Pareto Sets ! \n Be sure to insert an UtopicSol and References points" << endl;

		vector<vector<vector<double> > > vParetoSet;
		vector<vector<double> > paretoSetRef;

		for (int iM = 0; iM < vInputModel.size(); iM++)
		{
			cout << "Reading file: " << vInputModel[iM] << "....." << endl;

			Scanner scanner(new File(vInputModel[iM]));
			vector<vector<double> > paretoSet;
			while (scanner.hasNextDouble())
			{
				vector<double> fo;
				bool add = true;
				for (int o = 0; o < nOptObj; o++)
				{
					if (scanner.hasNextDouble())
					{
						fo.push_back(scanner.nextDouble());
					}
					else
					{
						add = false;
						break;
					}
				}
				if (add)
					paretoSet.push_back(fo);

			}
			moMetrics.unionSets(paretoSetRef, paretoSet);
			vParetoSet.push_back(paretoSet);
		}

		vParetoSet.push_back(paretoSetRef);

		vector<vector<double> > vIndicadoresQualidade;
		cout << "===================================" << endl;

		for (int nPS = 0; nPS < vParetoSet.size(); nPS++)
		{
			vector<vector<double> > paretoSet = vParetoSet[nPS];
			vector<double> indicadores;
			indicadores.push_back(paretoSet.size());
			//printVectorPareto(paretoSetRef);
			//cout << "conjuntoParetoAtual:" << endl;
			//printVectorPareto(paretoSet);
			double cardinalidade = moMetrics.cardinalite(paretoSet, paretoSetRef);
			double setCover = moMetrics.setCoverage(paretoSet, paretoSetRef);
			double spacing = moMetrics.spacing(paretoSet);
			indicadores.push_back(cardinalidade);
			indicadores.push_back(setCover);
			indicadores.push_back(spacing);

			double hv = -1;
			double delta = -1;
			if (utopicSol.size() > 0)
				delta = moMetrics.deltaMetric(paretoSet, utopicSol, true);

			if ((referencePointsHV.size() > 0) && (nPS < (vParetoSet.size() - 1)))
				hv = moMetrics.hipervolumeWithExecRequested(paretoSet, referencePointsHV, true);

			indicadores.push_back(delta);
			indicadores.push_back(hv);
			vIndicadoresQualidade.push_back(indicadores);

			if (nPS < (vParetoSet.size() - 1))
				cout << "instance: " << vInputModel[nPS] << endl;
			else
				cout << "instance: Pareto Reference" << endl;

			cout << "size: " << paretoSet.size() << "/" << paretoSetRef.size() << endl;
			cout << "card: " << cardinalidade << endl;
			cout << "CS: " << setCover << endl;
			cout << "spacing: " << spacing << endl;
			cout << "delta: " << delta << endl;
			cout << "hv: " << hv << "\n" << endl;
		}

		cout << "printing pareto REF" << endl;
		cout << paretoSetRef << endl;
		cout << "printing Indicadores" << endl;
		cout.precision(10);
		cout << vIndicadoresQualidade << endl;

		getchar();

	}

// 2

};

#endif /* READPARETOSETS_HPP_ */
