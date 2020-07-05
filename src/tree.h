#ifndef TREE_H_B_AND_P
#define TREE_H_B_AND_P

#include "node.h"
#include "data.h"
#include "master.h"
#include "price.h"

#include <ilcplex/ilocplex.h>

#include <list>
#include <stack>
#include <queue>
#include <vector>
using namespace std;

class Tree
{
public:
	Tree(Data *in);
	~Tree();
	double search();
	void branch(Node &no, pair<int, int> &ofspringCandidates);
	pair<int, int> solve(Node &no, bool isRoot = false);
	void debug(Master *, Node &node);
	friend ostream& operator<<(ostream &out,const Tree &t);

private:
	pair<int, int> none;
	double integerSolution;
	vector< vector<int> > storage;
	Master *m;
	Data *in;
	list<Node> myTree;
};

#endif