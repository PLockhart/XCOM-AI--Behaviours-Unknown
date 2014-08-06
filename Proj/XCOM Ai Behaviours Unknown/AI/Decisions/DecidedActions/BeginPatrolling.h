#ifndef BeginPatrollingH
#define BeginPatrollingH

#include "../../Decision.h"
#include "../../DecisionTree.h"

class BeginPatrolling : public DecisionAction {

private:
	int _minComraderyForBound;	//min comradary needed for characters to begin bounding overwatch together

public:
	BeginPatrolling::BeginPatrolling(DecisionTree * tree, int minComraderyForBound);
	Action* Run();

private:
	Action* CreateBoundWithTeammate(AICharacter * teammate);
};

#endif