#ifndef IsInCoverH
#define IsInCoverH

#include "../Decision.h"
#include "../Action.h"
class DecisionTree;

class IsInCover : public Decision {

	//Methods
	//===================================================
public:
	IsInCover::IsInCover(DecisionTree * tree);
	Action* Run();
};

#endif