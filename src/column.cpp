#include "column.h"

Column::Column(Data *data, Master *master, Price *price) : d(data),
														   m(master),
														   p(price)
{
}

void Column::solve()
{
	cout << "Solving" << endl;
	while (true)
	{
		if (time(0) % 60 == 0)
			cout << '.';
		m->solve();

		for (int i = 0; i < d->nItems(); i++)
			p->setDual(i, m->getDual(i));

		p->solve();

		if (p->reducedCost() > -EPSILON)
			break;
		else
			m->addColumn(p->newColumn());
	}
	cout << "Done" << endl;
}

void Column::result()
{
	m->solveIP();
	cout << "Solution status: " << m->getStatus() << endl;
	m->getObjValue();
}