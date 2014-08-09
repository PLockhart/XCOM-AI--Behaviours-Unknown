#ifndef DecisionH
#define DecisionH

#include "Action.h"
class DecisionTree;

class Decision {

	//Variables
	//===================================================
public:
	DecisionTree * Tree;	//The overall tree this decision is part of
	Decision * TrueBranch;	//The node to resolve if this decision was true
	Decision * FalseBranch;	//The node to resolve if this decision was false

	//Methods
	//===================================================
public:
	Decision::Decision(DecisionTree * tree);
	Decision::~Decision();
	virtual Action* run() = 0;	//Action returned must be deleted if not used
};

//For the end of a path in a decision tree
class DecisionAction : public Decision {

	//Methods
	//===================================================
public:
	DecisionAction::DecisionAction(DecisionTree * tree);
	virtual Action* run() = 0;
};

#endif