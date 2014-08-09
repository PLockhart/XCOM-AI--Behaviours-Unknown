#ifndef AmmoCheckH
#define AmmoCheckH

#include "../Decision.h"
#include "../Action.h"
class DecisionTree;

class HasMinAmmo : public Decision {

	//Variables
	//===================================================
public:
	float _minPercent;	//The min percent that the user needs to have in their weapon

	//Methods
	//===================================================
public:
	HasMinAmmo::HasMinAmmo(DecisionTree * tree, float minPercent);
	Action* run();
};

#endif