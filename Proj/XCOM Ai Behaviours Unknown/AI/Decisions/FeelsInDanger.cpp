#include "FeelsInDanger.h"

#include "../DecisionTree.h"
#include "../AICharacter.h"
#include "../Team.h"
#include "../../Characters and Weapons/BaseWeapon.h"

//Checks to see if this character feels like they are in too much trouble
FeelsInDanger::FeelsInDanger(DecisionTree * tree, float damageConstant)
: Decision(tree) {

	_damageConstant = damageConstant;
}

Action* FeelsInDanger::Run() {

	//the accumulated threat from all the enemies
	float accThreat = 0;

	//the average threat that we deal to all engageable enemies
	float ourAverageThreatToEnemies = 0;

	//loop through all the visible enemies
	for (int i = 0; i < (int)Tree->Character->VisibleEnemies.size(); i++) {

		AICharacter * loopedEnemy = Tree->Character->VisibleEnemies[i];

		//accumulate its threat to the tree's character and our threat to them
		accThreat = loopedEnemy->Weapon->GetAccuracyToCharIncCover(Tree->Character) * loopedEnemy->Weapon->BulletsPerShot;
		ourAverageThreatToEnemies = Tree->Character->Weapon->GetAccuracyToCharIncCover(loopedEnemy) * Tree->Character->Weapon->BulletsPerShot;
	}

	float statAverage = (Tree->Character->Boldness + (float)Tree->Character->Aggression) / 2;

	//average out the accumulated threat we are to other enemies
	ourAverageThreatToEnemies /= (int)Tree->Character->VisibleEnemies.size();
	float averageEnemyThreat = accThreat / (int)Tree->Character->VisibleEnemies.size();

	//reduce the enemy threat if they are a lone enemy and we have teammates left
	if (Tree->CharTeam->GetTrackedEnemies().size() == 1 && Tree->CharTeam->GetTeammatesOf(Tree->Character).size() > 0)
		averageEnemyThreat /= 100;

	//take the average threat comparing it to the average boldness/aggression * constant
	//if greater than the character and constant, then character feels in danger
	if (statAverage * _damageConstant < averageEnemyThreat - ourAverageThreatToEnemies) {

		Tree->Log("Feels scared");
		return TrueBranch->Run();
	}

	else {

		Tree->Log("Holding nerve");
		return FalseBranch->Run();
	}
}