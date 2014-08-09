#ifndef FindNearCombatCoverH
#define FindNearCombatCoverH

#include "../../Decision.h"
#include "../../DecisionTree.h"

class FindNearCombatCover : public DecisionAction {

private:
	int _depth;	//Depth which the map will be flooded around the character to find cover

public:
	FindNearCombatCover::FindNearCombatCover(DecisionTree * tree, int depth);
	Action* run();

private:
	void removeVectorElement(vector<Tile*> &theVector, int &loopIndex);
};

#endif