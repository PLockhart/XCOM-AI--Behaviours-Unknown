#include "NearbyCombatCover.h"

#include "../DecisionTree.h"
#include "../Team.h"
#include "../AICharacter.h"
#include "../../Characters and Weapons/BaseWeapon.h"
#include "../PathFinding.h"
#include "../../Containers/DataContainers.h"

//increasing this makes the character picker closer tiles to the enemy
const float NEARBY_COMBAT_COVER_AGGRESSION_CONST = 3.5f;

//Checks to see if there is nearby cover for this character to move to and still engage enemies
NearbyCombatCover::NearbyCombatCover(DecisionTree * tree, int tileDepth)
: Decision(tree) {

	_tileDepth = tileDepth;
}


Action* NearbyCombatCover::run() {

	//flood the map to the given depth
	vector<Tile*> possiblePoints;
	PathFinding::floodMap(possiblePoints, Tree->Character->CurrentTile, _tileDepth);

	vector<EnemyPositionHistory> enemyPositions = Tree->CharTeam->EnemyPositionHist;

	for (int i = 0; i < (int)possiblePoints.size(); i++) {

		//remove any tiles that are occupied
		if (possiblePoints[i]->IsOccupied == true) {

			possiblePoints.erase(possiblePoints.begin() + i);
			i--;
			continue;
		}

		bool canEngage = false;

		//remove any tiles that can't engage an enemy at an acceptable level
		for (int j = 0; j < (int)enemyPositions.size(); j++) {

			float accuracyOverDist = Tree->Character->Weapon->getAccuracyFromPlaceToTarget(possiblePoints[i]->Position, enemyPositions[j].TheTile->Position);

			//reduce accuracy if in cover from the possible tile position
			if (enemyPositions[j].TheTile->isInCoverFrom(possiblePoints[i]->Position) == true)
				accuracyOverDist /= Tree->Character->Weapon->CoverPenalty;

			//remove tiles that the character cannot engage from
			if (accuracyOverDist >= Tree->Character->Aggression * NEARBY_COMBAT_COVER_AGGRESSION_CONST) {

				canEngage = true;
			}
		}

		//remove if we couldn't engage any enemies from it
		if (canEngage == false) {

			possiblePoints.erase(possiblePoints.begin() + i);
			i--;
			continue;
		}

		//only consider tiles that actually provide cover from visible enemies
		bool providesCover = false;

		for (int j = 0; j < (int)enemyPositions.size(); j++) {

			if (possiblePoints[i]->isInCoverFrom(enemyPositions[j].TheTile->Position) == true) {

				providesCover = true;
				break;
			}
		}

		//remove tiles that didn't provide any cover
		if (providesCover == false) {

			possiblePoints.erase(possiblePoints.begin() + i);
			i--;
			continue;
		}
	}

	//if there were was at least 1 tile that we can move to, to protect ourselves against an enemy then return true
	if (possiblePoints.size() > 0) {

		Tree->log("Nearby cover available");
		return TrueBranch->run();
	}
	else {

		Tree->log("No nearby cover");
		return FalseBranch->run();
	}
}