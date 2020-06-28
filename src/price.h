#ifndef PRICE_H_B_AND_P
#define PRICE_H_B_AND_P

#include "data.h"

// CPLEX
#include <ilcplex/ilocplex.h>

// STL
#include <iostream>
using namespace std;

class Price
{
public:
	Price(Data *input);
	void setDual(IloNum var, IloNum dual);
	friend ostream &operator<<(ostream &out, Price &p);
	void solve();
	IloNum reducedCost();
	IloNumArray newColumn();
	void addSameBinConstraint(vector<pair<int,int> > pair);
private:
	Data *in;
	IloNumArray price;
	IloModel patGen;
	IloNumVarArray x;
	IloObjective ReducedCost;
	IloCplex priceSolver;
	IloNumArray newPatt;
};

#endif