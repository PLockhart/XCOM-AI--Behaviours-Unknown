#ifndef IsUnderLowThreatH
#define IsUnderLowThreatH

#include "../Decision.h"
#include "../Action.h"
class DecisionTree;

class IsUnderLowThreat : public Decision {

private:
	float _threatConstant;

	//Methods
	//===================================================
public:
	IsUnderLowThreat::IsUnderLowThreat(DecisionTree * tree, float threatConstant);
	Action* run();
};

#endif