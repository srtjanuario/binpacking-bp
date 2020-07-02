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

pair<int, int> Tree::solve(Node &no, bool isRoot)
{
	try
	{
		/* * *
		 * Column generation phase
		 * * */
		while (true)
		{
			m->solve(no);
			
			Price p(this->in, no);	
			
			if (!m->isFeasible())
				break;

			forn(in->nItems())
				p.setDual(i, m->getDual(i));

			cout<<p<<endl;
			p.solve();
			exit(0);

			// Stop if pricing is not feasible
			if (!p.isFeasible())	
				return m->reset();
			

			// Break if there is no more columns to add
			cout<<p.reducedCost()<<endl;
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

		cout << Lambda_value.getSize() << " - ";
		forn(Lambda_value.getSize())
			cout << Lambda_value[i] << " ";
		cout << endl;

		/**
		 * Reasons to Bound (it does not apply to the root node):
		 * 	1) The current bound is worse than the best integer solution.
		 * 	2) We are using artificial values.
		 * */
		if (!isRoot)
		{
			// 1) The current bound is worse than the best integer solution.
			if (ceil(m->getObjValue()) >= integerSolution)
				return m->reset();

			// 2) We are still using artificial values.
			forn(in->nItems()) {
				if (Lambda_value[i] > EPSILON) {
					cout<<"We are using artificial values."<<endl;
					return m->reset();
				}
			}
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
				if (abs(0.5 - accumulation) < mostFractional)
				{
					branchingPair = {i, j};
					mostFractional = abs(0.5 - accumulation);
				}
			}
		}

		// cout<<mostFractional<<endl;

		// Check if we found an integer solution?
		if (abs(0.5 - mostFractional) < EPSILON)
		{
			integerSolution = (m->binPackingSolver.getObjValue() < integerSolution) ? m->binPackingSolver.getObjValue() : integerSolution;
			return m->reset();
		}

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
	return none;
}

double Tree::search()
{
	Node root;
	pair<int, int> ofspringCandidates;
	ofspringCandidates = solve(root, true);

	Node nj, ns;
	ns = root;
	nj = root;

	nj.together_.push_back(ofspringCandidates);
	ns.conflict_.push_back(ofspringCandidates);

	myTree.push_back(ns);
	myTree.push_back(nj);

	while (!myTree.empty())
	{
		// cout<<myTree.size()<<endl;
		Node nutella = myTree.front();
		myTree.pop_front();
		ofspringCandidates = solve(nutella);
		if (ofspringCandidates != none)
		{
			// cout<<ofspringCandidates.first<<" "<<ofspringCandidates.second<<endl;
			Node nj, ns;
			ns = nutella;
			nj = nutella;

			nj.together_.push_back(ofspringCandidates);
			ns.conflict_.push_back(ofspringCandidates);

			myTree.push_back(ns);
			myTree.push_back(nj);
		}
	}
	return integerSolution;
}

bool Tree::bound(){

	return false;
}