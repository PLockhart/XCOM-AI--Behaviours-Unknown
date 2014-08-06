#include "Decision.h"

//creates a decision note, that belongs to a tree
//a decision will either return the result of its left branch or right branch
Decision::Decision(DecisionTree * tree) {

	Tree = tree;
	TrueBranch = 0;
	FalseBranch = 0;
}

//when deconstructing a decision node, delete either side of its branches if it was assigned
Decision::~Decision() {

	if (TrueBranch != 0)
		delete TrueBranch;

	if (FalseBranch != 0)
		delete FalseBranch;
}

//a decision action is a decision node where at least 1 of the branches returns an action itself (ie the end of a branch)
DecisionAction::DecisionAction(DecisionTree * tree)
: Decision(tree) {

}