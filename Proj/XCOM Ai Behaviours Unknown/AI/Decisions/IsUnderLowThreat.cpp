#include "IsUnderLowThreat.h"

#include "../DecisionTree.h"
#include "../AICharacter.h"
#include "../Team.h"
#include "../../Characters and Weapons/BaseWeapon.h"

#include "../../Characters and Weapons/Shotgun.h"

//Checks to see if the user is in good cover from enemies. Characters may disregard on account boldness
IsUnderLowThreat::IsUnderLowThreat(DecisionTree * tree, float threatConstant)
: Decision(tree) {

	_threatConstant = threatConstant;
}

//checks to see if the actor has an acceptable amount of ammo
Action* IsUnderLowThreat::run() {

	//get all the visible enemies
	vector<AICharacter*> visibleEnemies = Tree->Character->VisibleEnemies;

	int totalEnemies = (int)visibleEnemies.size();
	//the num of enemies that are a threat to us from flanking 
	int threateningEnemies = 0;
	//the accumulated threat of enemies that are attacking the player without cover
	float accThreat = 0;

	for (int i = 0; i < (int)visibleEnemies.size(); i++) {

		AICharacter * loopedEnemy = visibleEnemies[i];

		//check to see if we are in cover from that enemy
		if (Tree->Character->isInCoverFromAttack(loopedEnemy->Position) == false) {

			//check to see if they can engage us
			float accuracyToChar = loopedEnemy->Weapon->getAccuracyToCharIncCover(Tree->Character);
			if (accuracyToChar > 0) {

				threateningEnemies++;
				accThreat += accuracyToChar * loopedEnemy->Weapon->BulletsPerShot;
			}
		}
	}	

	//work out our threat to the enemies
	float ourHighestThreat = 0;

	for (int i = 0; i < (int)visibleEnemies.size(); i++) {

		AICharacter * loopedEnemy = visibleEnemies[i];

		//if we have > 0 accuracy to them, then we can threaten them
		float accuracyToChar = Tree->Character->Weapon->getAccuracyToCharIncCover(loopedEnemy);
		if (accuracyToChar > 0) {

			//work out our threat to them
			float threat = accuracyToChar * Tree->Character->Weapon->BulletsPerShot;

			//if greater than the previous highest threat, set the new highest
			if (threat > ourHighestThreat)
				ourHighestThreat = threat;
		}
	}

	if (accThreat == 0) {

		Tree->log("Protected from enemies");
		return TrueBranch->run();
	}

	//reduce the enemy threat if they are a lone enemy and we have teammates left
	if (Tree->CharTeam->getTrackedEnemies().size() == 1 && Tree->CharTeam->getTeammatesOf(Tree->Character).size() > 0)
		accThreat /= 100;

	//weight the characters boldness and the number of flankers against if we are more of a threat than they are to us
	//if greater than the threat, then we are in good cover as far as the character is concerned
	if (_threatConstant + Tree->Character->Boldness * 10 > accThreat - ourHighestThreat) {

		Tree->log("Doesn't feel too threatened");
		return TrueBranch->run();
	}

	else {
		
		Tree->log("Feels threatened");
		return FalseBranch->run();
	}
}