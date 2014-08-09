#include "IsInCover.h"
#include "../DecisionTree.h"
#include "../AICharacter.h"
#include "../../Characters and Weapons/BaseWeapon.h"

//Checks to see if this character is in cover from an enemy
IsInCover::IsInCover(DecisionTree * tree)
: Decision(tree) {

}

Action* IsInCover::run() {

	for (int i = 0; i < (int)Tree->Character->VisibleEnemies.size(); i++) {

		//check to see if we are in cover from at least 1 enemy that we are protected against
		if (Tree->Character->VisibleEnemies[i]->Weapon->isInRange(Tree->Character->Position) &&
			Tree->Character->isInCoverFromAttack(Tree->Character->VisibleEnemies[i]->Position)) {

				return TrueBranch->run();
		}
	}

	return FalseBranch->run();
}