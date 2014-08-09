#ifndef HasCooperativeTeammatesH
#define HasCooperativeTeammatesH

#include "../Decision.h"
#include "../Action.h"
class DecisionTree;

class HasCooperativeTeammates : public Decision {

	//Variables
	//===================================================
public:
	float _minComradery;	//The min comradery needed to pass the check

	//Methods
	//===================================================
public:
	HasCooperativeTeammates::HasCooperativeTeammates(DecisionTree * tree, float minComradery);
	Action* run();
};

#endif