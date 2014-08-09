#ifndef FeelsInDangerH
#define FeelsInDangerH

#include "../Decision.h"
#include "../Action.h"
class DecisionTree;

class FeelsInDanger : public Decision {

protected:
	float _damageConstant;

	//Methods
	//===================================================
public:
	FeelsInDanger::FeelsInDanger(DecisionTree * tree, float damageConstant);
	Action* run();
};

#endif