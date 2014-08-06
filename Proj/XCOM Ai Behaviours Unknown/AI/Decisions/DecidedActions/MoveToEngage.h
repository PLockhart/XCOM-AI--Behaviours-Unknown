#ifndef MoveToEngageH
#define MoveToEngageH

#include "../../Decision.h"
#include "../../DecisionTree.h"

class MoveToEngage : public DecisionAction {

public:
	MoveToEngage::MoveToEngage(DecisionTree * tree);
	Action* Run();
};

#endif