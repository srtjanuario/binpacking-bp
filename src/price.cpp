#include "price.h"

Price::Price(Data *input) : price(input->env(), input->nItems()),
							newPatt(input->env(), input->nItems()),
							patGen(input->env()),
							x(input->env(), input->nItems(), 0.0, 1, ILOINT),
							priceSolver(input->env())
{
	this->in = input;
	patGen.setName("Knapsack");

	for (int i = 0; i < in->nItems(); i++)
		x[i].setName(("x_" + to_string(i)).c_str());

	ReducedCost = IloAdd(patGen, IloMinimize(in->env(), 1));
	ReducedCost.setName("Obj");
	patGen.add(IloScalProd(in->itemWeight(), x) <= in->binCapacity());

	priceSolver.extract(patGen);
	priceSolver.setOut(in->env().getNullStream());
}

void Price::addSameBinConstraint(vector<pair<int, int>> &pair)
{
	for (auto i : pair)
	{
		x[i.first].setLB(1.0);
		x[i.second].setLB(1.0);
	}
}

IloNumArray Price::newColumn()
{
	this->priceSolver.getValues(newPatt, x);
	return newPatt;
}

IloNum Price::reducedCost()
{
	return priceSolver.getValue(ReducedCost);
}

void Price::solve()
{
	ReducedCost.setLinearCoefs(x, price);
	priceSolver.solve();
}

ostream &operator<<(ostream &out, Price &p)
{
	out << p.patGen << endl;
	// for (int i = 0; i < m.in->nItems(); i++)
	// {
	// 	out << m.Lambda[i];
	// 	if (i < m.in->nItems() - 1)
	// 		out << ", ";
	// }
	// out << endl;
	return out;
}

void Price::setDual(IloNum var, IloNum dual)
{
	price[var] = -dual;
}
