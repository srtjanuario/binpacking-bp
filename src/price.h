#ifndef PRICE_H_B_AND_P
#define PRICE_H_B_AND_P

#include "data.h"
#include "node.h"
#include "minknap.h"

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
	friend class Tree;

	// Create a single model
	Price(Data *input);

	~Price();

	// Create a model with extra constraints available in Node no
	Price(Data *input, Node &no);

	// Returns a string that contains information about the model status
	string getStatus();

	// Returns true if priceSolver is feasible
	bool isFeasible();
	
	// Inicialize the variables of the price model with values from the dual of the master problem
	void setDual(IloNum var, IloNum dual);
	
	friend ostream &operator<<(ostream &out, Price &p);
	void solve(bool isRoot = false);
	IloNum reducedCost(bool isRoot = false);
	double rc;
private:
	int* p;
	int* w;
	int* xFast;
	IloModel pricingModel;
	Data *in;
	IloNumArray price;	
	IloBoolVarArray x;
	IloObjective ReducedCost;
	IloCplex priceSolver;
};

#endif