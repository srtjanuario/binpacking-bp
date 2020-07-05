#include "tree.h"

#define EPSILON 1e-6
#define forn(n) for (int i = 0; i < (n); i++)

Tree::Tree(Data *input) : in(input)
{
	m = new Master(in);
	none = {0, 0};
	integerSolution = numeric_limits<double>::infinity();
}

Tree::~Tree()
{
	delete m;
}

void Tree::debug(Master *m, Node &node)
{
	cout << "J -> ";
	for (auto n : node.together_)
		printf("(%d,%d) ", n.first, n.second);
	cout << endl;
	cout << "S -> ";
	for (auto n : node.conflict_)
		printf("(%d,%d) ", n.first, n.second);
	cout << endl;

	IloNumArray Lambda_value(in->env(), m->Lambda.getSize());
	m->binPackingSolver.getValues(Lambda_value, m->Lambda);

	cout << Lambda_value.getSize() << " ~ ";
	forn(Lambda_value.getSize())
			cout
		<< Lambda_value[i] << " ";
	cout << endl;

	for (int i = 0; i < m->bin.size(); i++)
	{
		if (Lambda_value[i] == 1)
			cout << "Bin [" << i << "] = ";
		else
			cout << "    [" << i << "] = ";
		for (int j = 0; j < m->bin.size(); j++)
		{
			if (m->bin[i][j])
				cout << j << " ";
		}
		cout << endl;
	}
	if (Lambda_value.getSize() >= 10)
	{
		cout << m->masterBinPacking << endl;
		cout << m->Lambda << endl;
	}
}

pair<int, int> Tree::solve(Node &no, bool isRoot)
{
	/* * *
	* Restricted pricing problem
	* * */
	Price p(this->in, no);
	try
	{
		m->updateBranchingRules(no);
		
		/* * *
		 * Column generation phase
		 * * */
		while (true)
		{
			// Solver master problem
			m->solve();

			// Get dual variables
			forn(in->nItems())
				p.setDual(i, m->getDual(i));

			// Solve pricing
			p.solve();

			// Check it there is a new column
			if (p.reducedCost() > -EPSILON)
				break;

			// Add column and memorize the new items
			m->addColumn(p);
		}

		/* * *
		 * Branching phase
		 * * */
		IloNumArray Lambda_value(in->env(), m->Lambda.getSize());
		m->binPackingSolver.getValues(Lambda_value, m->Lambda);

		/**
		 * Reasons to Bound (it does not apply to the root node):
		 * 	1) The current bound is worse than the best integer solution.
		 * 	2) We are using artificial values.
		 * */
		if (!isRoot)
		{
			// 1) The current bound is worse than the best integer solution.
			if (ceil(m->getObjValue() - EPSILON) - integerSolution >= 0)
				return m->reset();
			// 2) We are still using artificial values.
			forn(in->nItems())
				if (Lambda_value[i] > EPSILON)		
					return m->reset();
		}

		double mostFractional = std::numeric_limits<double>::infinity();
		std::pair<int, int> branchingPair;
		// One i for each item
		for (int i = 0; i < in->nItems(); i++)
		{
			// One j for each item, so i < j
			for (int j = i + 1; j < in->nItems(); j++)
			{
				double accumulation = 0;
				// One k for each new bin/column
				for (int k = in->nItems(); k < m->bin.size(); k++)
					/* * *
					 * Every lambda represents a set of items
					 * If two items are in the same bin and the 
					 * lambda value is fractional we have to help 
					 * the solver to verify if they should stay 
					 * together in some bin or separated
					 * * */
					if (m->bin[k][i] == true && m->bin[k][j] == true)
						accumulation += Lambda_value[k];
				/* * *
				 * Choose itens that are in the same bin, but 
				 * their fractional values are closer to 0.5
				 * * */
				if (fabs(0.5 - accumulation) < mostFractional)
				{
					branchingPair = {i, j};
					mostFractional = fabs(0.5 - accumulation);
				}
			}
		}

		// Check if we found an integer solution?
		if (fabs(0.5 - mostFractional) < EPSILON)
		{
			integerSolution = (m->binPackingSolver.getObjValue() < integerSolution) ? m->binPackingSolver.getObjValue() : integerSolution;
			storage.resize(integerSolution);
			for(int k = 0,j=0; k < Lambda_value.getSize(); k++){
				if(Lambda_value[k] > 0.9){
					for (int i = 0; i < in->nItems(); i++)
						if(m->bin[k][i] == true)
							storage[j].push_back(i);
					j++;
				}
			}
			return m->reset();
		}

		// Clean master for the next node
		m->reset();

		return branchingPair;
	}
	catch (IloException &ex)
	{
		cerr << "File" << __FILE__ << "-> " << __LINE__ << ": " << ex << endl;
	}
	catch (...)
	{
		cout << "Unknown error in file " << __FILE__ << endl;
	}
	// Supress warning: control may reach end of non-void function [-Wreturn-type]
	cout << "Warning: control reached end of non-void function [-Wreturn-type]" << endl;
	return none;
}

void Tree::branch(Node& no, pair<int, int> &ofspringCandidates){
	Node nj, ns;
	ns = no;
	nj = no;

	nj.together_.push_back(ofspringCandidates);
	ns.conflict_.push_back(ofspringCandidates);

	myTree.push_back(ns);
	myTree.push_back(nj);
}


double Tree::search()
{
	Node root;
	pair<int, int> ofspringCandidates;
	ofspringCandidates = solve(root, true);
	branch(root,ofspringCandidates);

	while (!myTree.empty())
	{
		Node nutella = myTree.back();
		myTree.pop_back();
		ofspringCandidates = solve(nutella);
		if (ofspringCandidates != none)
			branch(nutella,ofspringCandidates);
	}
	return integerSolution;
}

ostream &operator<<(ostream &out, const Tree &t)
{
	out << t.integerSolution <<" bins"<<endl;
	int b = 1;
	for(auto bin: t.storage){
		cout<<"Bin["<<b<<"] = ";
		for(auto i:bin)
			cout<<i+1<<" ";
		cout<<endl;
		b++;
	}
	return out;
}
