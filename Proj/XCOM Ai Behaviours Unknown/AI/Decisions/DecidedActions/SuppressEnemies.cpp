#include "SuppressEnemies.h"

#include "../../DecisionTree.h"
#include "../../AICharacter.h"
#include "../../Team.h"
#include "../../../Characters and Weapons/BaseWeapon.h"
#include "../../Raycast.h"
#include "../../Actions/SuppressAction.h"

//Character decides if they should suppress enemies that are attacking a friendly unit. If not, it returns the false branch of the tree
SuppressEnemies::SuppressEnemies(DecisionTree * tree)
: DecisionAction(tree) {

}

Action* SuppressEnemies::Run() {

	vector<AICharacter*> teammates = Tree->CharTeam->GetTeammatesOf(Tree->Character);

	//if the weapon we are holding can't suppress, then false branch
	if (Tree->Character->Weapon->CanSuppress() == false) {

		Tree->Log("Can't suppress");
		return FalseBranch->Run();
	}

	//if we have no teammates to suppress for, then don't bother
	else if (teammates.size() == 0) {

		Tree->Log("No teammates to supress for");
		return FalseBranch->Run();
	}
	
	//if we are on our own, we have no reason to suppress
	else if (Tree->Character->Comradery == 0) {

		Tree->Log("Doesn't care for teammates");
		return FalseBranch->Run();
	}

	//get the total enemy threat to this tree's character
	float ourEnemyThreat = GetAssumedInfulenceForTile(Tree->Character->CurrentTile);

	//the enemies that this team can see
	vector<AICharacter*> enemies = Tree->Character->VisibleEnemies;

	//work out the most threatened ally from the enemies that we can target
	float highestAllyThreat = 0;
	AICharacter * highestThreatenedAlly = teammates[0];

	//looping through all our teammates
	for (int i = 0; i < (int)teammates.size(); i++) {

		float loopedAllyThreat = 0;

		//loop against all the enemies we can personally see (ie ones we can supress)
		for (int j = 0; j < (int)enemies.size(); j++) {

			if (Raycast::CastRay(enemies[j]->CurrentTile, teammates[i]->CurrentTile) == true) {

				//accumulate all of their threats to the character
				loopedAllyThreat += enemies[j]->Weapon->GetAccuracyToCharIncCover(teammates[i]) * enemies[j]->Weapon->BulletsPerShot;
			}
		}

		//if the accumlated threat to this looped teammate is more than the previous highest, set the new highest
		if (loopedAllyThreat >= highestAllyThreat) {

			highestAllyThreat = loopedAllyThreat;
			highestThreatenedAlly = teammates[i];
		}
	}

	//if the tree's character decides it is more threanted that the ally, return false
	if (ourEnemyThreat * Tree->Character->Aggression >= highestAllyThreat * Tree->Character->Comradery)
		return FalseBranch->Run();

	//the enemies that are attacking this tree's character
	vector<AttackData> ourAttackers = Tree->Character->GetAttackers();

	//remove any enemies that are suppressed or are suppressing us
	for (int i = 0; i < (int)enemies.size(); i++) {

		//remove any enemies that are already suppressed
		if (enemies[i]->IsSuppressed == true) {

			enemies.erase(enemies.begin() + i);
			i--;
			continue;
		}

		bool isBeingSurpressed = false;

		//remove any enemies that are suppressing us
		for (int j = 0; j < (int)ourAttackers.size(); j++) {

			if (ourAttackers[j].AttackType == kSuppressing && ourAttackers[j].Attacker == enemies[i]) {

				isBeingSurpressed = true;
				break;
			}
		}

		if (isBeingSurpressed == true) {

			enemies.erase(enemies.begin() + i);
			i--;
			continue;
		}

		//remove any enemies that cannot be seen/engaged
		if (Tree->Character->Weapon->IsInRange(enemies[i]->Position) == false ||
			Raycast::CastRay(Tree->Character->CurrentTile, enemies[i]->CurrentTile) == false) {

			enemies.erase(enemies.begin() + i);
			i--;
			continue;
		}
	}

	//if there are no more enemies we can target, return false
	if (enemies.size() == 0) {

		Tree->Log("No worthy candidates for suppression");
		return FalseBranch->Run();
	}

	//find the most threatening enemy
	float highestThreat = 0;
	AICharacter * mostThreateningEnemy = enemies[0];

	//loop through all the targetable enemies and compare their threat to all of our allies
	//finding the most threatening enemy
	for (int i = 0; i < (int)enemies.size(); i++) {

		for (int j = 0; j < (int)teammates.size(); j++) {

			//if the enemy can engage the teammate, then check its threat against the teammate
			if (Raycast::CastRay(enemies[i]->CurrentTile, teammates[j]->CurrentTile) == true) {

				float enemyThreat = enemies[i]->Weapon->GetAccuracyToCharIncCover(teammates[j]) * enemies[i]->Weapon->BulletsPerShot;

				//if the current enemy is more threatening than the previously worked out enemy, then set the new most threatening enemy
				if (enemyThreat > highestThreat) {

					highestThreat = enemyThreat;
					mostThreateningEnemy = enemies[i];
				}
			}
		}
	}

	Tree->Log("Suppressing");
	return new SuppressAction(Tree->Character, mostThreateningEnemy, Tree->Character->Aggression);
}

//Returns the infulence from enemy teams on the specified tile. Always from this tree's character's perspective
float SuppressEnemies::GetAssumedInfulenceForTile(Tile * theTile) {

	vector<InfulenceData> enemyAssumedInfulence = Tree->CharTeam->GetAssumedEnemyInfulencedTiles();

	//work out our threat from enemies by find the tile in the assumed enemy infulence that the character is on
	for (int i = 0; i < (int)enemyAssumedInfulence.size(); i++) {

		if (enemyAssumedInfulence[i].TheTile == theTile) {

			return enemyAssumedInfulence[i].Infulence;
		}
	}

	return 0;
}