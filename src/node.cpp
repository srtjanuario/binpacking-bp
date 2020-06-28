#include "node.h"
#include "price.h"

Node::Node(){

}

pair<int,int> Node::pricing(){
	Price p(this->in);
}