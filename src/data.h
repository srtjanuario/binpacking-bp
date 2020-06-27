#ifndef DATA_H_B_AND_P
#define DATA_H_B_AND_P

// CPLEX
#include <ilcplex/ilocplex.h>

//STL
#include <iostream>
using namespace std;

class Data
{
public:
	Data(int argc, char **argv,IloEnv *env_);
	IloEnv env() const;
	IloNum binCapacity() const;
	IloNum nItems() const;
	IloNum itemWeight(IloNum i) const;
	IloNumArray itemWeight();

	friend ostream& operator<<(ostream &out,const Data &d);
private:
	IloEnv env_;
	IloNum binCapacity_;
	IloNum nItems_;
	IloNumArray itemWeight_;
};
#endif