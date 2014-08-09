#include "EnemyPresense.h"

#include "../DecisionTree.h"
#include "../Team.h"
#include "../../Containers/DataContainers.h"

//checks to see if we know of any locations that enemies were standing, or any enemies that we can currently see
EnemyPresense::EnemyPresense(DecisionTree * tree) 
: Decision(tree) {

}

Action* EnemyPresense::run() {

	//if there are no recorded enemy positions that we know of then return false
	if ((int)Tree->CharTeam->EnemyPositionHist.size() != 0)
		return TrueBranch->run();

	return FalseBranch->run();
}