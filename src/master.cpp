#include "master.h"

#define EPSILON 1e-6
#define M 1e6
#define forn(n) for (int i = 0; i < (n); i++)

Master::Master(Data *input) : masterBinPacking(input->env()),
							  Lambda(input->env(), input->nItems(), 0, IloInfinity),
							  Assignment(input->env()),
							  binPackingSolver(input->env())
{
	this->in = input;
	MIP = false;
	IloExpr sum(input->env());
	for (int i = 0; i < input->nItems(); i++)
	{
		Lambda[i].setName(("l" + to_string(i)).c_str());
		Assignment.add(Lambda[i] == 1); // <-- I don't know why I need to assign Lambda[i] == 1  \o\ \o| |o| |o/ /o/
		sum += M * Lambda[i];
	}
	masterBinPacking.setName("Master");
	binsUsed = IloAdd(masterBinPacking, IloMinimize(in->env(), sum));
	binsUsed.setName("Bins");
	masterBinPacking.add(Assignment);
	binPackingSolver.extract(masterBinPacking);
	binPackingSolver.setOut(in->env().getNullStream());

	// One item in each bin
	bin = vector<vector<bool>>(in->nItems(), vector<bool>(in->nItems(), false));

	// Item i is in bin i
	for (int i = 0; i < in->nItems(); i++)
		bin[i][i] = true;
}

void Master::addColumn(Price &p, bool isRoot)
{
	// Adjusting pricing variables
	IloNumArray x_values(in->env(), in->nItems());
	if (isRoot)
	{
		for (int i = 0; i < in->nItems(); i++)
			x_values[i] = p.xFast[i];
	}
	else
	{
		p.priceSolver.getValues(x_values, p.x);
		for (int i = 0; i < x_values.getSize(); i++)
			x_values[i] = (x_values[i] > 0.9) ? 1 : 0;
	}

	// Adding the new column
	IloNumVar newColumn(binsUsed(1) + Assignment(x_values), 0, IloInfinity);
	newColumn.setName(("l" + to_string(Lambda.getSize())).c_str());
	Lambda.add(newColumn);

	// Memorize the new colum
	bin.push_back(vector<bool>(in->nItems(), false));
	for (int i = 0; i < x_values.getSize(); i++)
		bin.back()[i] = (x_values[i] > 0.9) ? true : false;
}

string Master::getStatus()
{
	stringstream buffer;
	buffer << binPackingSolver.getStatus();
	return buffer.str();
}

bool Master::isFeasible()
{
	return !(this->binPackingSolver.getCplexStatus() == IloCplex::Infeasible);
}

void Master::getDual(Price &price, bool isRoot)
{
	if (isRoot)
	{
		for (int i = 0; i < in->nItems(); i++)
			price.p[i] = (binPackingSolver.getDual(Assignment[i]) > 0) ? in->factor * binPackingSolver.getDual(Assignment[i]) : 0;
	}
	else
	{
		forn(in->nItems())
			price.setDual(i, binPackingSolver.getDual(Assignment[i]));
	}
}

void Master::updateBranchingRules(Node &no)
{

	/**
	 * Include branching rules
	 * */
	for (int b = in->nItems(); b < bin.size(); b++)
	{
		Lambda[b].setUB(IloInfinity);
		/**
		 * Problem: two items must stay together, either
		 * 			inside bin b or not
		 * Solution: Eliminate the column in which only
		 * 			 one item is in bin b. (XOR)
		 * */
		for (auto &item : no.together_)
			if (bin[b][item.first] ^ bin[b][item.second])
				Lambda[b].setUB(0.0);

		/**
		 * Problem: two items cannot be in the same bin b
		 * Solution: Eliminate the column in which both
		 * 			 items are in the same bin b. (AND)
		 * */
		for (auto &item : no.conflict_)
			if (bin[b][item.first] && bin[b][item.second])
				Lambda[b].setUB(0.0);
	}
}

// Solver the Master problem
void Master::solve()
{
	binPackingSolver.solve();
}

// Convert variable so ILOBOOL and solve the problem
void Master::solveIP()
{
	if (!MIP)
	{
		masterBinPacking.add(IloConversion(in->env(), Lambda, ILOBOOL));
		MIP = true;
	}
	binPackingSolver.solve();
}

double Master::getObjValue()
{
	return binPackingSolver.getObjValue();
}

void Master::debug()
{
	cout << *this << endl;
	cout << "Using " << binPackingSolver.getObjValue() << " bins" << endl;
	for (IloNum j = 0; j < Lambda.getSize(); j++)
		cout << "  L_" << j << " = " << binPackingSolver.getValue(Lambda[j]) << endl;
	if (!MIP)
	{
		for (IloNum i = 0; i < Assignment.getSize(); i++)
			cout << "  A_" << i << " = " << binPackingSolver.getDual(Assignment[i]) << endl;
	}
}

ostream &operator<<(ostream &out, Master &m)
{
	out << m.masterBinPacking << endl;
	for (int i = 0; i < m.in->nItems(); i++)
	{
		out << m.Lambda[i];
		if (i < m.in->nItems() - 1)
			out << ", ";
	}
	out << endl;
	return out;
}