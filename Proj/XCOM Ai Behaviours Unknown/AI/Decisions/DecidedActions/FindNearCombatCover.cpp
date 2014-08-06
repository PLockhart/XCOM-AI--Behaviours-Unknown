#include "FindNearCombatCover.h"

#include <algorithm>
#include <sstream>

#include "../../Team.h"
#include "../../DecisionTree.h"
#include "../../AICharacter.h"
#include "../../PathFinding.h"
#include "../../Actions/TacticalMove.h"
#include "../../../Containers/DataContainers.h"
#include "../../../Characters and Weapons/BaseWeapon.h"

//Returns a move action to nearest cover if avaliable, otherwise it returns the false path of the decision tree
//the depth refers to the amount of tiles away that can be consdered to find good cover
FindNearCombatCover::FindNearCombatCover(DecisionTree * tree, int depth)
	: DecisionAction(tree) {

	_depth = depth;
}

Action* FindNearCombatCover::Run() {

	vector<AICharacter*> visibleEnemies = Tree->Character->VisibleEnemies;
	vector<InfulenceData> enemiesInfulence = Tree->CharTeam->GetAssumedEnemyInfulencedTiles();

	//get all the tiles within the depth for us to consider
	vector<Tile*> nearbyTiles;
	PathFinding::FloodMap(nearbyTiles, Tree->Character->CurrentTile, _depth);

	vector<Tile*> possiblePoints;

	//combine all of the enemy line of sights that are within move range, avoiding duplicates
	for (int i = 0; i < (int)visibleEnemies.size(); i++) {

		vector<Tile*> enemyLOS = visibleEnemies[i]->CurrentTile->GetLosTiles();

		for (int j = 0; j < (int)enemyLOS.size(); j++) {

			//only consider tiles we can move to and are part of the nearby tiles
			if (enemyLOS[j]->CoverType == kOpen &&
				std::find(nearbyTiles.begin(), nearbyTiles.end(), enemyLOS[j]) != nearbyTiles.end()
				) {

					//don't add the same tile twice
				if (std::find(possiblePoints.begin(), possiblePoints.end(), enemyLOS[j]) == possiblePoints.end())
					possiblePoints.push_back(enemyLOS[j]);
			}
		}
	}
	
	for (int i = 0; i < (int)possiblePoints.size(); i++) {

		//don't consider any tiles currently occupied
		if (possiblePoints[i]->IsOccupied == true) {

			RemoveVectorElement(possiblePoints, i);
			continue;
		}

		//only consider tiles that actually provide cover from visible enemies
		bool providesCover = false;

		for (int j = 0; j < (int)visibleEnemies.size(); j++) {

			if (possiblePoints[i]->IsInCoverFrom(visibleEnemies[j]->Position) == true) {

				providesCover = true;
				break;
			}
		}

		//remove tiles that didn't provide any cover
		if (providesCover == false) {

			RemoveVectorElement(possiblePoints, i);
			continue;
		}

		bool canEngage = false;

		//loop through all the visible enemies and make sure this tile can engage one of them to an acceptable level
		for (int j = 0; j < (int)visibleEnemies.size(); j++) {

			float accuracyOverDist = Tree->Character->Weapon->GetAccuracyFromPlaceToTarget(possiblePoints[i]->Position, visibleEnemies[j]->Position);

			//reduce accuracy if in cover from the possible tile position
			if (visibleEnemies[j]->CurrentTile->IsInCoverFrom(possiblePoints[i]->Position) == true)
				accuracyOverDist /= Tree->Character->Weapon->CoverPenalty;

			//remove tiles that the character cannot engage from
			if (accuracyOverDist >= Tree->Character->Aggression * 3.5) {

				canEngage = true;
				possiblePoints[i]->Weighting -= accuracyOverDist * Tree->Character->Weapon->BulletsPerShot;
			}
		}

		//remove if we couldn't engage any enemies from it
		if (canEngage == false) {

			RemoveVectorElement(possiblePoints, i);
			continue;
		}
		
		//decrease the weighting by the distance we'd have to travel by to get to it
		float guessConstant = (10 - Tree->Character->Aggression) / 5;	//the higher the constant the further a character will be likely to move

		int xCost = abs(Tree->Character->CurrentTile->X - possiblePoints[i]->X);
		int yCost = abs(Tree->Character->CurrentTile->Y - possiblePoints[i]->Y);
		int Hcost = (xCost + yCost) * 10 * guessConstant;
		possiblePoints[i]->Weighting += Hcost;

		//decrease the weighting by the amount of damage we'd expect to take
		vector<InfulenceData> enemyInfulence = Tree->CharTeam->GetAssumedEnemyInfulencedTiles();

		//increase the infulence of any possible points that overlap with enemy infulence
		for (int j = 0; j < (int)enemyInfulence.size(); j++) {

			InfulenceData enemyData = enemyInfulence[i];

			if (possiblePoints[i] == enemyData.TheTile) {

				//modify the weighting, taking into account the boldness of the character		//divide by constant
				possiblePoints[i]->Weighting += enemyData.Infulence * (10 - (Tree->Character->Boldness)) / 1;//5;
				break;
			}
		}
	}

	if (possiblePoints.size() == 0) {

		Tree->Log("No nearby cover");
		return FalseBranch->Run();
	}

	// find the tile with the lowest weighting
	Tile * bestTile = possiblePoints[0];
	for (int i = 1; i < (int)possiblePoints.size(); i++) {

		if (bestTile->Weighting > possiblePoints[i]->Weighting) {
			bestTile = possiblePoints[i];
			i = 1;
		}
	}

	std::stringstream output;
	output << "Moving to nearby cover ";
	output << bestTile->X;
	output << ", ";
	output << bestTile->Y;
	Tree->Log(output.str());

	//reset all of the tile's weighting for any later pathfinding
	for (int i = 0; i < (int)possiblePoints.size(); i++) {
		possiblePoints[i]->Weighting = 0;
	}

	//tactically move to that location
	return new TacticalMove(Tree->Character, bestTile, Tree->Character->Aggression);
}

void FindNearCombatCover::RemoveVectorElement(vector<Tile*> &theVector, int &loopIndex) {

	theVector.erase(theVector.begin() + loopIndex);
	loopIndex--;
}