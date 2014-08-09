#ifndef EngageEnemiesH
#define EngageEnemiesH

#include "../../Decision.h"
#include "../../DecisionTree.h"

class EngageEnemies : public DecisionAction {

public:
	EngageEnemies::EngageEnemies(DecisionTree * tree);
	Action* run();
};

#endif