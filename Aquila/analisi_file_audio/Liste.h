#pragma once
#ifndef NODE_H
#define NODE_H 

class Node {
	Node *link_prec;
	int data;
	Node *link_succ;

public:
	Node(int d, Node *lp = 0, Node *ls = 0);
	~Node();
	int GetData()const;
	Node * GetLinkPrec()const;
	Node * GetLinkSucc()const;
	void SetData(int d);
	void SetLinkSucc(Node *l);
};
#endif