#include "price.h"

Price::Price(Data *input) : price(input->env(), input->nItems()),
							newPatt(input->env(), input->nItems()),
							patGen(input->env()),
							Use(input->env(), input->nItems(), 0.0, 1, ILOINT),
							priceSolver(input->env())
{
	this->in = input;
	patGen.setName("Knapsack");

	for (int i = 0; i < in->nItems(); i++)
		Use[i].setName(("I_" + to_string(i)).c_str());

	ReducedCost = IloAdd(patGen, IloMinimize(in->env(), 1));
	ReducedCost.setName("Obj");
	patGen.add(IloScalProd(in->itemWeight(), Use) <= in->binCapacity());

	priceSolver.extract(patGen);
	priceSolver.setOut(in->env().getNullStream());
}

void Price::addSameBinConstraint(vector<pair<int, int>> &pair)
{
	for (auto i : pair)
	{
		Use[i.first].setLB(1.0);
		Use[i.second].setLB(1.0);
	}
}

IloNumArray Price::newColumn()
{
	this->priceSolver.getValues(newPatt, Use);
	return newPatt;
}

IloNum Price::reducedCost()
{
	return priceSolver.getValue(ReducedCost);
}

void Price::solve()
{
	ReducedCost.setLinearCoefs(Use, price);
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
