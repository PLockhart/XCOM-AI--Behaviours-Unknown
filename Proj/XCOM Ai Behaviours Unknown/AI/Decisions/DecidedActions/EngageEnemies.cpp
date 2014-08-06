#include "EngageEnemies.h"
#include "../../Team.h"
#include "../../DecisionTree.h"
#include "../../AICharacter.h"
#include "../../Actions/Attackaction.h"
#include "../../../Characters and Weapons/BaseWeapon.h"

EngageEnemies::EngageEnemies(DecisionTree * tree)
	: DecisionAction(tree) {

}

//Character engages the enemy it has the best chacne of hitting
Action* EngageEnemies::Run() {

	Tree->Log("Firing at enemies");

	vector<AICharacter*> targetableEnemies;

	//check to see if any of the characters are within range
	for (int i = 0; i < (int)Tree->Character->VisibleEnemies.size(); i++) {

		AICharacter * loopedEnemy = Tree->Character->VisibleEnemies[i];

		//if in range of the weapon, add it to the targetable list
		if (Tree->Character->Weapon->GetAccuracyToTarget(loopedEnemy->Position) > 0)
			targetableEnemies.push_back(loopedEnemy);
	}

	//find the most easily targetable enemy
	float bestTargetWeighting = 0;
	AICharacter * bstTrgt = targetableEnemies[0];

	//loop through the targetable enemies and see if there is a better enemy to target
	//this takes into account the enemies threat to us
	for (int i = 0; i < (int)targetableEnemies.size(); i++) {

		AICharacter * loopedEnemy = targetableEnemies[i];
		//work out our accuracy to the enemy
		float accToLoop = Tree->Character->Weapon->GetAccuracyToCharIncCover(loopedEnemy);

		//work out the enemies threat to us
		float threatFromLoop = loopedEnemy->Weapon->GetAccuracyToCharIncCover(Tree->Character) * loopedEnemy->Weapon->BulletsPerShot;
		float totalWeighting = accToLoop + threatFromLoop / 10; //divide threat by a constant

		//if we have a new higher threat, switch to that target
		if (totalWeighting > bestTargetWeighting) {

			bstTrgt = loopedEnemy;
			bestTargetWeighting = accToLoop;
		}
	}

	//engage the best target, with the aggression of the character as its priority
	return new AttackAction(Tree->Character, bstTrgt, Tree->Character->Aggression);
}