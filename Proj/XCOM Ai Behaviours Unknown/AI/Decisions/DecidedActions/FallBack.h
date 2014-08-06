#ifndef FallBackH
#define FallBackH

#include "../../Decision.h"
#include "../../DecisionTree.h"

class FallBack : public DecisionAction {

protected:
	int _tilesDeep;	//the amount of tiles away that can be considered to fallback to

public:
	FallBack::FallBack(DecisionTree * tree, int tilesDeep);
	Action* Run();
};

#endif