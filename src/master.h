#ifndef MASTER_H_B_AND_P
#define MASTER_H_B_AND_P

#include "data.h"

// CPLEX
#include <ilcplex/ilocplex.h>

// STL
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

class Master
{
public:
	Master(Data *input);
	void solve();
	void solveIP();
	
	void debug();
	IloNum getDual(IloNum var);
	string getStatus();
	void addColumn(IloNumArray column);
	friend ostream &operator<<(ostream &out, Master &m);
private:
	Data *in;
	IloModel masterBinPacking;
	IloNumVarArray Lambda;
	IloRangeArray Assignment;
	IloObjective binsUsed;
	IloCplex binPackingSolver;
	bool MIP;
};
#endif