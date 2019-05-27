#include "stdafx.h"
#include "Liste.h"
#include <iostream>

using namespace std;

Node::Node(int d, Node *lp, Node *ls) {
	data = d;
	link_prec = lp;
	link_succ = ls;
}
Node::~Node() {
	cout << "~Node(" << data << ")" << endl;
}
int Node::GetData()const {
	return data;
}
Node * Node::GetLinkPrec()const {
	return link_prec;
}
Node * Node::GetLinkSucc()const {
	return link_succ;
}
void Node::SetData(int d) {
	data = d;
}
void Node::SetLinkSucc(Node *l) {
		link_succ = l;
}