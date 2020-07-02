#ifndef NODE_H_B_AND_P
#define NODE_H_B_AND_P

#include <iostream>
#include <vector>
using namespace std;

struct Node
{
	vector<pair<int, int> > together_;
	vector<pair<int, int> > conflict_;
};

#endif