#ifndef PRICE_H_B_AND_P
#define PRICE_H_B_AND_P

#include "data.h"
#include "node.h"

// CPLEX
#include <ilcplex/ilocplex.h>

// STL
#include <iostream>
using namespace std;

class Price
{
public:

	// Give full access to Master class 
	friend class Master;

	// Create a single model
	Price(Data *input);

	// Create a model with extra constraints available in Node no
	Price(Data *input, Node &no);

	// Returns a string that contains information about the model status
	string getStatus();

	// Returns true if priceSolver is feasible
	bool isFeasible();
	
	// Inicialize the variables of the price model with values from the dual of the master problem
	void setDual(IloNum var, IloNum dual);
	
	friend ostream &operator<<(ostream &out, Price &p);
	void solve();
	IloNum reducedCost();
	IloNumArray newColumn();
	void addSameBinConstraint(vector<pair<int,int> > pair);
private:
	IloModel patGen;
	Data *in;
	IloNumArray price;	
	IloNumVarArray x;
	IloObjective ReducedCost;
	IloCplex priceSolver;
	IloNumArray newPatt;
};

#endif