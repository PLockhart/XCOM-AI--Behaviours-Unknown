#include "DisregardsSafety.h"

#include "../DecisionTree.h"
#include "../AICharacter.h"
#include "../../Characters and Weapons/BaseWeapon.h"

//Checks to see if this character is willing to disregard safety in order to do damage
//the constant regards to the unacceptable threat when compared to our threat - enemy threat
DisregardsSafety::DisregardsSafety(DecisionTree * tree, float threatConstant)
: Decision(tree) {

	_threatConstant = threatConstant;
}

Action* DisregardsSafety::run() {

	//the accumulated threat from all the enemies
	float accThreat = 0;

	//loop through all the visible enemies
	for (int i = 0; i < (int)Tree->Character->VisibleEnemies.size(); i++) {

		AICharacter * loopedEnemy = Tree->Character->VisibleEnemies[i];

		//accumulate its threat to the tree's character
		accThreat = loopedEnemy->Weapon->getAccuracyToCharIncCover(Tree->Character) * loopedEnemy->Weapon->BulletsPerShot;
	}

	float statAverage = (Tree->Character->Boldness + (float)Tree->Character->Aggression) / 2;

	//take the average threat comparing it to the average boldness/aggression * constant
	//if greater than the character and constant, then character feels in danger
	if (statAverage * _threatConstant > accThreat / (int)Tree->Character->VisibleEnemies.size()) {

		Tree->log("Throwing caution to the wind");
		return TrueBranch->run();
	}

	else {

		Tree->log("Not willing to take risks");
		return FalseBranch->run();
	}
}