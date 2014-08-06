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
Action* ShouldMoveInForKill::Run() {

	//can't move in for a kill if we are suppressing an enemy
	if (Tree->Character->HasAction == true) {

		if (dynamic_cast<SuppressAction*>(Tree->Character->CurrentAct->FrontAction())) {

			Tree->Log("Can't move in for kill while suppressing");
			return FalseBranch->Run();
		}
	}

	//if the character doesn't have enough aggression to meet the requirement...
	if (Tree->Character->Aggression < _minAggression) {

		Tree->Log("Not aggressive enough for assault");
		return FalseBranch->Run();
	}

	//filter out enemies that aren't suppressed and within range
	vector<AICharacter*> suppressedEnemies = Tree->CharTeam->GetVisibleEnemies();

	for (int i = 0; i < (int)suppressedEnemies.size(); i++) {

		if (suppressedEnemies[i]->IsSuppressed == false || suppressedEnemies[i]->Position.distance(Tree->Character->Position) > _maxRange) {

			suppressedEnemies.erase(suppressedEnemies.begin() + i);
			i--;
			continue;
		}
	}

	//if there are no suppressed enemies we can engage, then no enemies to assault
	if (suppressedEnemies.size() == 0) {

		Tree->Log("No suppressed enemies to assault");
		return FalseBranch->Run();
	}

	//check to see if the character is already engaging someone else well
	for (int i = 0; i < (int)Tree->Character->VisibleEnemies.size(); i++) {

		if (Tree->Character->Weapon->GetAccuracyToCharIncCover(Tree->Character->VisibleEnemies[i]) >= 75) {

			Tree->Log("Already engaging a good target");
			return FalseBranch->Run();
		}
	}


	//if we do have candidates to assault, see if we aren't already in a good spot for engaging them
	for (int i = 0; i < (int)suppressedEnemies.size(); i++) {

		if (Tree->Character->Weapon->GetAccuracyToCharIncCover(suppressedEnemies[i]) >= 75) {

			Tree->Log("Already in good assaulting position");
			return FalseBranch->Run();
		}
	}

	Tree->Log("Enemies vunerable to assault");
	return TrueBranch->Run();
}