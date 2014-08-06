#ifndef SinglePatrolH
#define SinglePatrolH

#include "../../Decision.h"

class SinglePatrol : public DecisionAction {

public:
	SinglePatrol::SinglePatrol(DecisionTree * tree);
	Action* Run();
};

#endif