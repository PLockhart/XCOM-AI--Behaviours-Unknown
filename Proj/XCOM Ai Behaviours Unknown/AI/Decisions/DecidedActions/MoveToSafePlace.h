#ifndef MoveToSafePlaceH
#define MoveToSafePlaceH

#include "../../Decision.h"
#include "../../DecisionTree.h"

class MoveToSafePlace : public DecisionAction {

public:
	MoveToSafePlace::MoveToSafePlace(DecisionTree * tree);
	Action* run();
};

#endif