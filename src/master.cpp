#include "master.h"

Master::Master(Data *input) : masterBinPacking(input->env()),
							  Lambda(input->env(), input->nItems(), 0, IloInfinity),
							  Assignment(input->env()),
							  binPackingSolver(input->env())
{
	this->in = input;
	MIP = false;
	for (int i = 0; i < input->nItems(); i++)
	{
		Lambda[i].setName(("L_" + to_string(i)).c_str());
		Assignment.add(Lambda[i] == 1); // <-- I don't know why I need this
	}
	masterBinPacking.setName("Master");
	binsUsed = IloAdd(masterBinPacking, IloMinimize(in->env(), IloSum(Lambda)));
	binsUsed.setName("Bins");
	masterBinPacking.add(Assignment);
	binPackingSolver.extract(masterBinPacking);
	binPackingSolver.setOut(in->env().getNullStream());
}

void Master::addColumn(IloNumArray column)
{
	Lambda.add(IloNumVar(binsUsed(1) + Assignment(column)));
}

string Master::getStatus()
{
	stringstream buffer;
	buffer << binPackingSolver.getStatus();
	return buffer.str();
}

IloNum Master::getDual(IloNum var)
{
	return binPackingSolver.getDual(Assignment[var]);
}

void Master::solve()
{
	binPackingSolver.solve();
}

void Master::solveIP()
{
	if (!MIP){
		masterBinPacking.add(IloConversion(in->env(), Lambda, ILOINT));
		MIP = true;
	}
	binPackingSolver.solve();
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