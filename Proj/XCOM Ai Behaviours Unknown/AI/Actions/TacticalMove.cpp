#include "TacticalMove.h"

#include <algorithm>

#include "../AICharacter.h"
#include "../PathFinding.h"
#include "../Team.h"

//Increasing this makes the AI less considerate of enemy threat. Decreasing this makes them worry about enemies more
const int ENEMY_BOLDNESS_MODIFIER_CONST = 10;
//The extra weighting given to a tile it if provides protection from gunfire
const int COVER_PROTECTION_CONST = 100;

//Moves tactically around cover avoiding enemy fire when possible
TacticalMove::TacticalMove(AICharacter * character, Tile * destination, int priority)
	:MoveAction(priority) {

	ActingCharacter = character;
	_originalDestination = destination;
}

//Gets the infulence of all the known enemies to this characters team and plots a safe path
bool TacticalMove::getPathwayData(vector<Tile*> &pathway, Tile * start, Tile * destination) {

	vector<InfulenceData> infulencedTiles = ActingCharacter->ParentTeam->getAssumedEnemyInfulencedTiles();

	//decrease the enemy's infulence according to our characters boldness
	float boldnessConst = 1;
	for (int i = 0; i < (int)infulencedTiles.size(); i++)
		infulencedTiles[i].Infulence *= (10 - (ActingCharacter->Boldness * boldnessConst)) / ENEMY_BOLDNESS_MODIFIER_CONST;	//divide by constant

	return PathFinding::getTacticalPathway(infulencedTiles, pathway, start, destination);
}

//searches for the best fallback position when immediate tiles cannot be moved to
Tile* TacticalMove::getIdealFallback() {

	//find all the tiles we can readily move to
	vector<Tile*> neighbours;
	int depth = 2;

	while (true) {

		neighbours.clear();

		PathFinding::floodMap(neighbours, ActingCharacter->CurrentTile, depth);

		//get the enemy infulence over these tiles
		vector<InfulenceData> enemyInfulence = ActingCharacter->ParentTeam->getAssumedEnemyInfulencedTiles();

		//modify the weighting by the enemies infulence over that tile
		for (int i = 0; i < (int)enemyInfulence.size(); i++) {

			for (int j = 0; j < (int)neighbours.size(); j++) {

				//exclude neighbours that are occupied
				if (neighbours[j]->IsOccupied == true) {

					neighbours.erase(neighbours.begin() + j);
					j--;
					continue;
				}

				if (enemyInfulence[i].TheTile == neighbours[j])
					//using the Fcost so we can use the tiles Sort Tile method
					neighbours[j]->FCost = enemyInfulence[i].Infulence;
			}
		}

		for (int i = 0; i < (int)neighbours.size(); i++) {

			Tile * loopedCandidate = neighbours[i];

			//if in cover from enemies, increase the weighting
			int protectionValue = COVER_PROTECTION_CONST;

			for (int j = 0; j < (int)ActingCharacter->VisibleEnemies.size(); j++) {

				if (loopedCandidate->isInCoverFrom(ActingCharacter->VisibleEnemies[j]->Position))
					loopedCandidate->FCost += protectionValue;
			}
		}

		//if we found a valid place to fall back to, break the while loop
		if ((int)neighbours.size() > 0)
			break;

		//otherwise, reset all the neighbours we looped over for this depth so the fcosts aren't carried over 
		//and increase depth to expand out more, so we redo the test
		else {

			for (int j = 0; j < (int)neighbours.size(); j++) {
				neighbours[j]->FCost = 0;
			}

			depth++;
		}
	}
	//pick the best neighbour to move to
	std::sort(neighbours.begin(), neighbours.end(), Tile::IsLower);
	Tile * best = neighbours[0];

	//reset the tiles FCost that we may have touched
	for (int j = 0; j < (int)neighbours.size(); j++) {
		neighbours[j]->FCost = 0;
	}

	return best;
}

bool TacticalMove::shouldGiveWayTo(Action * other) {

	//if they are of the same type..
	if (isSameKind(other) == true) {

		TacticalMove * derived = dynamic_cast<TacticalMove*>(other);

		if (derived->_originalDestination == _originalDestination)
			return false;

		if (other->Priority >= Priority)
			return true;

		return false;
	}

	//if not the same type, prioritise the action with the bigger priority
	else if (other->Priority >= Priority)
		return true;

	return false;
}

bool TacticalMove::isSameKind(Action * other) {

	if (TacticalMove * derived = dynamic_cast<TacticalMove*>(other))
		return true;

	return false;
}

std::string TacticalMove::toString() {

	return "Moving Tactically";
}