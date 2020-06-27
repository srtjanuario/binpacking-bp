#ifndef COLUMN_H_B_AND_P
#define COLUMN_H_B_AND_P

// USER
#include "data.h"
#include "master.h"
#include "price.h"

// CPLEX
#include <ilcplex/ilocplex.h>

//STL
#include <iostream>
using namespace std;

#define EPSILON 1e-6

class Column
{
public:
	Column(Data *data, Master *master, Price *price);
	void solve();
	void result();
private:
	Data *d;
	Master *m;
	Price *p;
};

#endif