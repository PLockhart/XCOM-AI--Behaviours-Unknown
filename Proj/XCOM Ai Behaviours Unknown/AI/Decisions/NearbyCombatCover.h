#ifndef NearbyCombatCoverH
#define NearbyCombatCoverH

#include "../Decision.h"
#include "../Action.h"
class DecisionTree;

class NearbyCombatCover : public Decision {

protected:
	int _tileDepth;	//the tiles deep around the character we consider to move to for cover

	//Methods
	//===================================================
public:
	NearbyCombatCover::NearbyCombatCover(DecisionTree * tree, int tileDepth);
	Action* run();
};

#endif