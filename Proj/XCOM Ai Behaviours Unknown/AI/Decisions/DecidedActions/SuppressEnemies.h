#ifndef SuppressEnemiesH
#define SuppressEnemiesH

#include "../../Decision.h"
#include "../../Action.h"
class DecisionTree;

//If there are enemies to engage, the best enemy is chosen to be surpressed. if not, then the false branch is returned
class SuppressEnemies : public DecisionAction {

	//Methods
	//===================================================
public:
	SuppressEnemies::SuppressEnemies(DecisionTree * tree);
	Action* Run();

private:
	float GetAssumedInfulenceForTile(Tile * theTile);
};

#endif