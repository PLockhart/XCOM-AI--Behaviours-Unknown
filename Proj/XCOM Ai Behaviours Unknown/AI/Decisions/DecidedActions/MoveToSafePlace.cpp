#include "MoveToSafePlace.h"
#include "../../DecisionTree.h"
#include "../../Team.h"
#include "../../AICharacter.h"
#include "../../Actions/TacticalMove.h"
#include "../../PathFinding.h"
#include "../../../Containers/DataContainers.h"

MoveToSafePlace::MoveToSafePlace(DecisionTree * tree)
	: DecisionAction(tree) {

}

//Character should fall back to the safest place they can
Action* MoveToSafePlace::run() {

	//flood the map of all the places we can move to within a resonable large range
	vector<Tile*> fallbackPositions;
	PathFinding::floodMap(fallbackPositions, Tree->Character->CurrentTile, Tree->Character->Speed * 5);

	//remove any possible fall back positions that are occupied by other characters
	for (int i = 0; i < (int)fallbackPositions.size(); i++) {
		
		if (fallbackPositions[i]->IsOccupied == true) {

			fallbackPositions.erase(fallbackPositions.begin() + i);
			i--;
		}
	}

	//get the assumed enemy infulence in the level
	vector<InfulenceData> enemyInfulence = Tree->CharTeam->getAssumedEnemyInfulencedTiles();
	vector<AICharacter*> enemies = Tree->CharTeam->getVisibleEnemies();

	for (int i = 0; i < (int)fallbackPositions.size(); i++) {

		//weight the tiles against the enemies infulence across them
		for (int j = 0; j < (int)enemyInfulence.size(); j++) {

			InfulenceData loopedInfulence = enemyInfulence[j];

			//check to see if the infulence is that of the looped fall back position
			if (fallbackPositions[i] == loopedInfulence.TheTile) {

				//increase the weighting
				fallbackPositions[i]->Weighting += loopedInfulence.Infulence;
				break;
			}
		}

		//weight the positions by cover it provides also
		int coverValue = 40;
		if (fallbackPositions[i]->CoverEast == true)
			fallbackPositions[i]->Weighting -= coverValue;
		if (fallbackPositions[i]->CoverWest == true)
			fallbackPositions[i]->Weighting -= coverValue;
		if (fallbackPositions[i]->CoverNorth == true)
			fallbackPositions[i]->Weighting -= coverValue;
		if (fallbackPositions[i]->CoverSouth == true)
			fallbackPositions[i]->Weighting -= coverValue;
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

	//create a priority based off the lack of aggression and boldness, averaged from 0 to 10
	int priority = (20 - (Tree->Character->Aggression + Tree->Character->Boldness)) / 2;

	Tree->log("Moving to safe position");
	return new TacticalMove(Tree->Character, bestTile, priority);
}