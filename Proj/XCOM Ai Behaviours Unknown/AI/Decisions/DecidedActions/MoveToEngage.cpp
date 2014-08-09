#include "MoveToEngage.h"
#include "../../DecisionTree.h"
#include "../../AICharacter.h"
#include "../../Team.h"
#include "../../../Level/Tile.h"
#include "../../../Characters and Weapons/BaseWeapon.h"
#include "../../../Containers/DataContainers.h"
#include "../../Actions/TacticalMove.h"
#include "../../PathFinding.h"

#include <iostream>
#include <sstream>

//increasing this value will mean the character will consider tiles further away
const float MOVE_TO_ENGAGE_MOVEMENT_CONST = 10;
//Increasing this will make the character move to more aggressive positions
const float MOVE_TO_ENGAGE_AGGRESSION_CONST = 2;
//Increasing this will make the character move aggressive against suppressed enemies
const float MOVE_TO_ENGAGE_SUPPRESSED_CONST = 9.5f;

//moves to engage the closest enemy we know of
MoveToEngage::MoveToEngage(DecisionTree * tree)
	: DecisionAction(tree) {

}

//Move the character to a position to engage enemies
Action* MoveToEngage::run() {

	//pick the closest enemy
	vector<EnemyPositionHistory> enemyHistory = Tree->CharTeam->EnemyPositionHist;
	EnemyPositionHistory nearestEnemyRecord = enemyHistory[0];

	//check the other enemies and see if there is a closer enemy
	for (int i = 1; i < (int)enemyHistory.size(); i++) {

		Tile * loopedTile = enemyHistory[i].TheTile;

		//if the looped enemy is closer that the previously calculated closest enemy, then 
		if (nearestEnemyRecord.TheTile->Position.distance(Tree->Character->Position) > loopedTile->Position.distance(Tree->Character->Position))
			nearestEnemyRecord = enemyHistory[i];
	}

	//find all the positions we can engage the target from
	//tiles will be sorted by weighting, where lowest is best
	vector<Tile*> possiblePoints = nearestEnemyRecord.TheTile->getLosTiles();

	//weight according to weapon accuracy from this position
	for (int i = 0; i < (int)possiblePoints.size(); i++) {

		float accuracyOverDist = Tree->Character->Weapon->getAccuracyFromPlaceToTarget(possiblePoints[i]->Position, nearestEnemyRecord.TheTile->Position);

		//reduce accuracy if in cover from the possible tile position
		if (nearestEnemyRecord.TheTile->isInCoverFrom(possiblePoints[i]->Position) == true)
			accuracyOverDist /= Tree->Character->Weapon->CoverPenalty;

		//remove tiles as possible movement positions if they AREN'T
		if (
			//within the characters acceptable range. Only have this expectation if the character isn't suppressed
			(accuracyOverDist <= Tree->Character->Aggression * MOVE_TO_ENGAGE_AGGRESSION_CONST && Tree->Character->IsSuppressed == false) ||
			//if the enemy is surpressed, and we can't take full advantage of it if we aren't surpressed ourselves
			(accuracyOverDist <= Tree->Character->Aggression * MOVE_TO_ENGAGE_SUPPRESSED_CONST && nearestEnemyRecord.TheEnemy->IsSuppressed == true && Tree->Character->IsSuppressed == false) ||
			//or can be shot from at all, regardless on whether the character is surpressed or not
			accuracyOverDist <= 0 ||
			//moveable to
			possiblePoints[i]->CoverType != kOpen || 
			//where the enemy is at this moment of time
			possiblePoints[i] == nearestEnemyRecord.TheTile ||
			//occupied by characters other than ourselves
			possiblePoints[i]->IsOccupied == true
			) {

				possiblePoints.erase(possiblePoints.begin() + i);
				i--;
		}

		//else if in range, decrease the weighting by what damage we'd expect to get on the target
		else {

			if (accuracyOverDist > 100)
				accuracyOverDist = 100;

			float infulence = accuracyOverDist * Tree->Character->Weapon->BulletsPerShot;

			//decrease weighting of tile by our damage, modified by our aggression
			possiblePoints[i]->Weighting -= infulence / (10 - Tree->Character->Aggression);

			//now increase weighting by how far we guess we have to move
			int xCost = abs(Tree->Character->CurrentTile->X - possiblePoints[i]->X);
			int yCost = abs(Tree->Character->CurrentTile->Y - possiblePoints[i]->Y);

			//guess the distance to the end tile
			float guessConstant = (10 - Tree->Character->Aggression) / MOVE_TO_ENGAGE_MOVEMENT_CONST;	//the higher the constant the further a character will be likely to move
			int Hcost = (xCost + yCost) * 10 * guessConstant;
			possiblePoints[i]->Weighting += Hcost;
		}
	}

	//weight the tiles against the exposure that enemies would have when firing at us from this position
	vector<InfulenceData> enemyInfulence = Tree->CharTeam->getAssumedEnemyInfulencedTiles();

	//increase the infulence of any possible points that overlap with enemy infulence
	for (int i = 0; i < (int)enemyInfulence.size(); i++) {

		InfulenceData enemyData = enemyInfulence[i];

		for (int j = 0; j < (int)possiblePoints.size(); j++) {

			Tile * possibleTile = possiblePoints[j];

			if (possibleTile == enemyData.TheTile) {

				//modify the weighting, taking into account the boldness of the character		//divide by constant
				possibleTile->Weighting += enemyData.Infulence * (10 - (Tree->Character->Boldness)) / 1;
			}
		}
	}

	// find the tile with the lowest weighting
	Tile * bestTile = possiblePoints[0];
	for (int i = 1; i < (int)possiblePoints.size(); i++) {

		if (bestTile->Weighting > possiblePoints[i]->Weighting) {
			bestTile = possiblePoints[i];
			i = 1;
		}
	}
	
	//reset all of the tile's weighting for any later pathfinding
	for (int i = 0; i < (int)possiblePoints.size(); i++) {
		possiblePoints[i]->Weighting = 0;
	}

	
	std::stringstream output;
	output << "Moving to engage from ";
	output << bestTile->X;
	output << ", ";
	output << bestTile->Y;
	Tree->log(output.str());
	

	//tactically move to that location
	return new TacticalMove(Tree->Character, bestTile, Tree->Character->Aggression);
}