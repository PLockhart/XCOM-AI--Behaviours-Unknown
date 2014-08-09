#include "ShouldMoveInForKill.h"
#include "../DecisionTree.h"
#include "../AICharacter.h"
#include "../Team.h"
#include "../../Characters and Weapons/BaseWeapon.h"
#include "../Actions/SuppressAction.h"

//Character decides if it should move into a strong position to engage suppressed enemies that are within a certain distance
ShouldMoveInForKill::ShouldMoveInForKill(DecisionTree * tree, float maxRange, float minAggression)
: Decision(tree) {

	_maxRange = maxRange;
	_minAggression = minAggression;
}

//checks to see if the character should move in to finish off the enemy
Action* ShouldMoveInForKill::run() {

	//can't move in for a kill if we are suppressing an enemy
	if (Tree->Character->HasAction == true) {

		if (dynamic_cast<SuppressAction*>(Tree->Character->CurrentAct->frontAction())) {

			Tree->log("Can't move in for kill while suppressing");
			return FalseBranch->run();
		}
	}

	//if the character doesn't have enough aggression to meet the requirement...
	if (Tree->Character->Aggression < _minAggression) {

		Tree->log("Not aggressive enough for assault");
		return FalseBranch->run();
	}

	//filter out enemies that aren't suppressed and within range
	vector<AICharacter*> suppressedEnemies = Tree->CharTeam->getVisibleEnemies();

	for (int i = 0; i < (int)suppressedEnemies.size(); i++) {

		if (suppressedEnemies[i]->IsSuppressed == false || suppressedEnemies[i]->Position.distance(Tree->Character->Position) > _maxRange) {

			suppressedEnemies.erase(suppressedEnemies.begin() + i);
			i--;
			continue;
		}
	}

	//if there are no suppressed enemies we can engage, then no enemies to assault
	if (suppressedEnemies.size() == 0) {

		Tree->log("No suppressed enemies to assault");
		return FalseBranch->run();
	}

	//check to see if the character is already engaging someone else well
	for (int i = 0; i < (int)Tree->Character->VisibleEnemies.size(); i++) {

		if (Tree->Character->Weapon->getAccuracyToCharIncCover(Tree->Character->VisibleEnemies[i]) >= 75) {

			Tree->log("Already engaging a good target");
			return FalseBranch->run();
		}
	}


	//if we do have candidates to assault, see if we aren't already in a good spot for engaging them
	for (int i = 0; i < (int)suppressedEnemies.size(); i++) {

		if (Tree->Character->Weapon->getAccuracyToCharIncCover(suppressedEnemies[i]) >= 75) {

			Tree->log("Already in good assaulting position");
			return FalseBranch->run();
		}
	}

	Tree->log("Enemies vunerable to assault");
	return TrueBranch->run();
}