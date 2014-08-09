#include "FallBack.h"

#include "../../DecisionTree.h"
#include "../../Team.h"
#include "../../AICharacter.h"
#include "../../Actions/MoveAction.h"
#include "../../PathFinding.h"
#include "../../../Containers/DataContainers.h"

//The penalty to a tile if the enemy will be flanking it. Decreasing this makes the AI not as concerned about being flanked
const int FALLBACK_FLANK_PENALTY_CONST = 1000;
//Tiles that are within this distance of an enemy are considered too close and will be penalised.
const int FALLBACK_ENEMY_TOO_CLOSE_CONST = 120;

//Creates a move action for the character to the safeest place from enemy fire
//only considering tiles a certain amount away
FallBack::FallBack(DecisionTree * tree, int tilesDeep)
	: DecisionAction(tree) {

	_tilesDeep = tilesDeep;
}

//Character should fall back to a safer position
Action* FallBack::run() {

	//flood the map of all the places we can move to within a long range
	vector<Tile*> fallbackPositions;
	PathFinding::floodMap(fallbackPositions, Tree->Character->CurrentTile, Tree->Character->Speed * _tilesDeep);

	for (int i = 0; i < (int)fallbackPositions.size(); i++) {
		
		//remove any possible fall back positions that are occupied by other characters
		if (fallbackPositions[i]->IsOccupied == true) {

			fallbackPositions.erase(fallbackPositions.begin() + i);
			i--;
		}
	}

	//get the assumed enemy infulence in the level
	vector<InfulenceData> enemyInfulence = Tree->CharTeam->getAssumedEnemyInfulencedTiles();

	//weight the tiles against the enemies infulence across them
	for (int i = 0; i < (int)fallbackPositions.size(); i++) {
		for (int j = 0; j < (int)enemyInfulence.size(); j++) {

			InfulenceData loopedInfulence = enemyInfulence[j];

			//check to see if the infulence is that of the looped fall back position
			if (fallbackPositions[i] == loopedInfulence.TheTile) {

				//increase the weighting
				fallbackPositions[i]->Weighting += loopedInfulence.Infulence;
				break;
			}
		}
	}

	vector<AICharacter*> visibleEnemies = Tree->CharTeam->getVisibleEnemies();

	//weight the possible positions by how far away they are from enemies
	for (int i = 0; i < (int)fallbackPositions.size(); i++) {

		//find the angle from the characters current position to the looped enemy
		Vector3 dir = fallbackPositions[i]->Position - Tree->Character->Position;

		//resolve vector into an angle, where 0 is facing up n degrees
		float angleToTile = ((atan2(dir.y, dir.x) * 180) / 3.1415926) + 90;

		for (int j = 0; j < (int)visibleEnemies.size(); j++) {

			float distance = visibleEnemies[j]->Position.distance(fallbackPositions[i]->Position);

			if (distance <= FALLBACK_ENEMY_TOO_CLOSE_CONST)
				fallbackPositions[i]->Weighting += distance * 10;

			else
				fallbackPositions[i]->Weighting -= visibleEnemies[j]->Position.distance(fallbackPositions[i]->Position) / 10;

			//find the angle from the characters current position to the looped enemy
			dir = visibleEnemies[j]->Position - Tree->Character->Position;

			//resolve vector into an angle, where 0 is facing up n degrees
			float angleToEnemy = ((atan2(dir.y, dir.x) * 180) / 3.1415926) + 90;

			//if the angle to the enemy is similair to the angle to the possible fallback position, then we are moving towards an enemy
			//and thus should be peanalised heavily
			if (Rotations::rotationsSimilair(angleToTile, angleToEnemy, 100) == true) {

				fallbackPositions[i]->Weighting += FALLBACK_FLANK_PENALTY_CONST;
			}
		}

		//now increase weighting by how far we guess we have to move
		int xCost = abs(Tree->Character->CurrentTile->X - fallbackPositions[i]->X);
		int yCost = abs(Tree->Character->CurrentTile->Y - fallbackPositions[i]->Y);

		//guess the distance to the end tile
		int Hcost = (xCost + yCost) * 10;
		fallbackPositions[i]->Weighting += Hcost * 10;
	}

	//loop through all of the possible points and find that with the lowest weighting
	Tile * bestTile = fallbackPositions[0];
	for (int i = 1; i < (int)fallbackPositions.size(); i++) {

		if (bestTile->Weighting > fallbackPositions[i]->Weighting) {
			bestTile = fallbackPositions[i];
			i = 1;
		}
	}

	//reset all of the tile's weighting for any later pathfinding
	for (int i = 0; i < (int)fallbackPositions.size(); i++) {
		fallbackPositions[i]->Weighting = 0;
	}

	Tree->log("Wants to fall back");
	return new MoveAction(Tree->Character, bestTile, 1);
}