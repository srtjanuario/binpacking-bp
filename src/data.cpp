#include "data.h"

Data::Data(int argc, char **argv, IloEnv *env) : itemWeight_(*env)
{
	if (argc < 2)
		throw invalid_argument("Plese, give me an input file");

	env_ = *env;
	
	ifstream in(argv[1]);
	if (in)
	{
		in >> nItems_;
		in >> binCapacity_;
		int weight;
		for (int i = 0; i < nItems_; i++)
		{
			in >> weight;
			itemWeight_.add(weight);
		}
	}
	else
		throw runtime_error("File not found");
}

IloEnv Data::env() const
{
	return this->env_;
}

IloNum Data::binCapacity() const
{
	return this->binCapacity_;
}

IloNum Data::nItems() const
{
	return this->nItems_;
}

IloNum Data::itemWeight(IloNum i) const
{
	return this->itemWeight_[i];
}

ostream &operator<<(ostream &out, const Data &d)
{
	out << d.nItems() << endl;
	out << d.binCapacity() << endl;
	for (int i = 0; i < d.nItems(); i++)
		cout << d.itemWeight(i) << endl;
	return out;
}
