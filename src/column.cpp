#include "column.h"

#define EPSILON 1e-6

Column::Column(Data *data, Master *master, Price *price) : d(data),
														   m(master),
														   p(price)
{
}

void Column::solve()
{
	cout << "Solving";
	int last = time(0);
	while (true)
	{
		cout << '.' << flush;

		m->solve();

		for (int i = 0; i < d->nItems(); i++)
			p->setDual(i, m->getDual(i));

		p->solve();

		if (p->reducedCost() > -EPSILON)
			break;
		else
			m->addColumn(*p);
	}
	cout << "Done" << endl;
}

void Column::result()
{
	m->solveIP();
	cout << "Solution status: " << m->getStatus() << endl;
	m->getObjValue();
}