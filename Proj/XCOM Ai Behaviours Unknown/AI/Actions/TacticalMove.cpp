#include "TacticalMove.h"

#include <algorithm>

#include "../AICharacter.h"
#include "../PathFinding.h"
#include "../Team.h"

//Moves tactically around cover avoiding enemy fire when possible
TacticalMove::TacticalMove(AICharacter * character, Tile * destination, int priority)
	:MoveAction(priority) {

	ActingCharacter = character;
	_originalDestination = destination;
}

//Gets the infulence of all the known enemies to this characters team and plots a safe path
bool TacticalMove::GetPathwayData(vector<Tile*> &pathway, Tile * start, Tile * destination) {

	vector<InfulenceData> infulencedTiles = ActingCharacter->ParentTeam->GetAssumedEnemyInfulencedTiles();

	//decrease the enemy's infulence according to our characters boldness
	float boldnessConst = 1;
	for (int i = 0; i < (int)infulencedTiles.size(); i++)
		infulencedTiles[i].Infulence *= (10 - (ActingCharacter->Boldness * boldnessConst)) / 10;	//divide by constant

	return PathFinding::GetTacticalPathway(infulencedTiles, pathway, start, destination);
}

//searches for the best fallback position when immediate tiles cannot be moved to
Tile* TacticalMove::GetIdealFallback() {

	//find all the tiles we can readily move to
	vector<Tile*> neighbours;
	int depth = 2;

	while (true) {

		neighbours.clear();

		PathFinding::FloodMap(neighbours, ActingCharacter->CurrentTile, depth);

		//get the enemy infulence over these tiles
		vector<InfulenceData> enemyInfulence = ActingCharacter->ParentTeam->GetAssumedEnemyInfulencedTiles();

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
			int protectionValue = 100;

			for (int j = 0; j < (int)ActingCharacter->VisibleEnemies.size(); j++) {

				if (loopedCandidate->IsInCoverFrom(ActingCharacter->VisibleEnemies[j]->Position))
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

bool TacticalMove::ShouldGiveWayTo(Action * other) {

	//if they are of the same type..
	if (IsSameKind(other) == true) {

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

bool TacticalMove::IsSameKind(Action * other) {

	if (TacticalMove * derived = dynamic_cast<TacticalMove*>(other))
		return true;

	return false;
}

std::string TacticalMove::ToString() {

	return "Moving Tactically";
}