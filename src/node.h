#ifndef NODE_H_B_AND_P
#define NODE_H_B_AND_P

#include "data.h"

#include <iostream>
#include <vector>
using namespace std;

class Node
{
public:
	Node();
	double bound() const;
	void boundSet();
	pair<int, int> pricing(Data *in);

private:
	vector<pair<int, int> > sameBin;
	double bound_;
};

struct CompareNode
{
	bool operator()(Node const &p1, Node const &p2)
	{
		// return "true" if "p1" is before "p2":
		return p1.bound() < p2.bound();
		return true;
		// else{

		// 	if((p1.subtour[p1.escolhido].size() - p1.escolhido) < (p2.subtour[p2.escolhido].size() - p2.escolhido))
		// 		return true;
		// 	else {
		// 		return (p1.subtour.size() < p2.subtour.size());
		// 	}
		// }
	}
};

#endif