#include "price.h"

Price::Price(Data *input) : price(input->env(), input->nItems()),
							newPatt(input->env(), input->nItems()),
							patGen(input->env()),
							x(input->env(), input->nItems()),
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

Price::Price(Data *input, Node &n):Price(input){
	
	// Force items to stay together...
	// It does not mean that the need to stay in the same bin :-)
	for (auto &i : n.together_)
		patGen.add(x[i.first] == x[i.second]);
		
	// Conflict constraint: at most one item can enter in this bin
	for (auto &i : n.conflict_)
		patGen.add(x[i.first] + x[i.second] <= 1);
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
	IloExpr profit(in->env());
	// profit+=1;
	for (int i = 0; i < in->nItems(); i++)
		profit -= price[i] * x[i] + 1;
	ReducedCost.setExpr(profit);
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

string Price::getStatus()
{
	stringstream buffer;
	buffer << priceSolver.getStatus();
	return buffer.str();
}

bool Price::isFeasible(){
	return !(this->priceSolver.getCplexStatus() == IloCplex::Infeasible);
}

void Price::setDual(IloNum var, IloNum dual)
{
	price[var] = -dual;
}
