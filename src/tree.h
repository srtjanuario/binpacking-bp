#ifndef TREE_H_B_AND_P
#define TREE_H_B_AND_P

#include "node.h"

#include <ilcplex/ilocplex.h>

#include <list>
#include <stack>
#include <queue>
#include <vector>
using namespace std;

class Tree
{
public:
	Tree();
	double deepFirstSearch();
	double breadthFirstSearch();
	double bestBoundSearch();
private:
	static double UB;
	list<Node> bfsTree;
	stack<Node> dfsTree;
	priority_queue<Node, vector<Node>, CompareNode> bestTree;
};

#endif