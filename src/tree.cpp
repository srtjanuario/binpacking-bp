#include "tree.h"

double Tree::UB = 0;

Tree::Tree()
{
}

bool Tree::bestBoundSearch()
{
	Node root;
	bestBoundTree.push(root);

	pair<int, int> ofspringCandidates;

	bestBoundTree.push(raiz);

	Node nutella;

	while (!bestBoundTree.empty())
	{
		// Select a node with the largest bound
		nutella = bestBoundTree.top();
		// if (!(nutella.bound < UB))
		// {
		// 	bestBoundTree.pop();
		// 	continue;
		// }
		ofspringCandidates = nutella.pricing();
		arvoreBest.pop();

		if (!(branchingPair.first == 0 && branchingPair.second == 0) && !(node_it->LB > p.bestInteger))
		{
			Node nj, ns;
			ns = *node_it;
			nj = *node_it;

			ns.is_root = false;
			nj.is_root = false;

			nj.juntos.push_back(branchingPair);
			nj.tipo_branch = true;

			ns.separados.push_back(branchingPair);
			ns.tipo_branch = false;

			tree.push_back(ns);
			tree.push_back(nj);
		}
	}

	return arvoreBest.empty();
}