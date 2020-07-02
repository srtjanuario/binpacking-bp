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
	Tree(Data* in);
	~Tree( );
	double search();
	pair<int, int> solve(Node& no, bool isRoot = false);
	
private:
	pair<int,int> none;
	int integerSolution;
	Master* m;
	Data* in;
	list<Node> myTree;
};

#endif