#include "EnemiesInRange.h"
#include "../DecisionTree.h"
#include "../AICharacter.h"
#include "../../Characters and Weapons/BaseWeapon.h"

//Checks to see if we can engage enemies based off our accuracy to them, which is modified by our aggression and the provided constant
EnemiesInRange::EnemiesInRange(DecisionTree * tree, float aggressionModifier)
: Decision(tree) {

	_aggressionModifier = aggressionModifier;
}

Action* EnemiesInRange::Run() {

	//loop through all the enemies this character can see
	for (int i = 0; i < (int)Tree->Character->VisibleEnemies.size(); i++) {

		AICharacter * loopedEnemy = Tree->Character->VisibleEnemies[i];
		
		//and check to see if we can engage any of the enemies with an acceptable amount of accuracy
		float accuracyToTarget = Tree->Character->Weapon->GetAccuracyToCharIncCover(loopedEnemy);
		if (accuracyToTarget > Tree->Character->Aggression * _aggressionModifier) {

			Tree->Log("Enemies in acceptable range");
			return TrueBranch->Run();
		}
	}

	Tree->Log("No enemies in acceptable range");
	return FalseBranch->Run();
}