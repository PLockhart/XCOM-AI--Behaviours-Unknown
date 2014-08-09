#include "Patrol.h"

#include <algorithm>

#include "../PathFinding.h"
#include "../../Containers/DataContainers.h"
#include "../AICharacter.h"
#include "../Team.h"
#include "../../Level/Tile.h"

//the max amount of recent tiles we keep track of. Increasing this will improve patrol quality but decrease performance
const int PATROL_MAX_TILE_CONST = 4;

//Creates a new patrol action, which will create new move actions to new points of high visibility
//Action finishes when an enemy is in sight
Patrol::Patrol(AICharacter * character, int priority)
: Action(priority) {

	ActingCharacter = character;
	_maxRecent = PATROL_MAX_TILE_CONST;
}

void Patrol::setup() {

	_isSetupFin = true;

	//add the current tile to the recent tiles list for the initial patrol movement
	_recentTiles.push_back(ActingCharacter->CurrentTile);
	
	//now find the best tile to move to next and create a move to that
	Tile * best = pickBestLOSTileFor(ActingCharacter);
	PatrolMove = new MoveAction(ActingCharacter, best, Priority);
}

//Performs the patrol action, by performing the PatrolMove
void Patrol::act(float dt, AICharacter * sender) {
	
	Action::act(dt, sender);

	MoverAct(dt, sender);

	//cancel the patrol if we have found an enemy
	if ((int)ActingCharacter->ParentTeam->EnemyPositionHist.size() > 0 && State != kCancelling)
		cancel();

	//if this patrol is cancelling
	if (State == kCancelling) {

		//check to see if the patrol move has finished cancelling
		if (PatrolMove->isActionComplete() == true) {
			//if so, we can finish this patrol action
			finished();
		}
	}
}

//The character who is moving does their movement routine
void Patrol::MoverAct(float dt, AICharacter * sender) {

	//see if we have are still moving to a new vantage point
	if (PatrolMove->isActionComplete() == false) {

		PatrolMove->act(dt, sender);
	}
	//else if the move to action has finished, we need to either end the patrol action or create a new one
	else {

		moverFinished();
	}
}

//event for when the mover has finished their movement to their destination
void Patrol::moverFinished() {

	delete PatrolMove;

	//if we aren't cancelling the patrol, begin a new patrol
	if (State != kCancelling) {

		Tile * best = pickBestLOSTileFor(ActingCharacter);
		PatrolMove = new MoveAction(ActingCharacter, best, Priority);
	}

	else
		finished();
}

//Gets all the best tiles to move to within a reasonable move distance. Returns a vector of candidates in order of best
vector<Tile*> Patrol::getBestLOSCandidatesFor(AICharacter * character) {

	vector<Tile*> moveCandidates;
	PathFinding::floodMap(moveCandidates, character->CurrentTile, character->Speed * 1.5f);

	vector<InfulenceData> teamInfulence = character->ParentTeam->getInfulencedTiles();
	vector<TileHistory> teamHistory = character->ParentTeam->getTeamTileHistory();

	//loop through all of the move candidates and sorting them by their score..
	//..which is calculated by weighting the tile's LOS with tiles that the team can see
	for (int i = 0; i < (int)moveCandidates.size(); i++) {

		Tile * loopedCandidate = moveCandidates[i];

		//don't consider tiles that are occupied
		if (loopedCandidate->IsOccupied == true) {

			moveCandidates.erase(moveCandidates.begin() + i);
			i--;
			continue;
		}

		vector<Tile*> losTiles = loopedCandidate->getLosTiles();
		loopedCandidate->Weighting = (int)losTiles.size();

		//score the tile based off the number of los tiles that are in common with the team's tile infulence
		for (int losIndx = 0; losIndx < (int)losTiles.size(); losIndx++) {

			//looping against the team's history
			for (int teamHist = 0; teamHist < (int)teamHistory.size(); teamHist++) {

				if (teamHistory[teamHist].TheTile == losTiles[losIndx])
					loopedCandidate->Weighting -= (1 - teamHistory[teamHist].Time) * 1;	//* by a constant
			}

		}

		//favour spots with cover
		int coverValue = 2;
		if (loopedCandidate->CoverEast == true)
			loopedCandidate->Weighting += coverValue;
		if (loopedCandidate->CoverWest == true)
			loopedCandidate->Weighting += coverValue;
		if (loopedCandidate->CoverNorth == true)
			loopedCandidate->Weighting += coverValue;
		if (loopedCandidate->CoverSouth == true)
			loopedCandidate->Weighting += coverValue;
	}

	//work out the average position of all the recent tiles
	Vector3 averagePos;
	int listSize = (int)_recentTiles.size();
	for (int i = 0; i < listSize; i++)
		averagePos = averagePos + _recentTiles[i]->Position;
	averagePos /= listSize;

	//give a better weighting to locations that are further away from the average recent positions
	for (int i = 0; i < (int)moveCandidates.size(); i++) {

		//if a move candidate was a recently visited tile, lower its worth
		if (std::find(_recentTiles.begin(), _recentTiles.end(), moveCandidates[i]) != _recentTiles.end())
			moveCandidates[i]->Weighting *= 0.7f;	//a constant value that works best for reduction

		else {
			float distance = moveCandidates[i]->Position.distance(averagePos);
			moveCandidates[i]->Weighting += distance * 0.01f;	//0.01f being a constant
		}
	}

	//having finished going through all of the candidates, sort them by highest weighting
	for (int i = 0; i < (int)moveCandidates.size(); i++) {

		for (int j = 0; j < (int)moveCandidates.size(); j++) {

			if (moveCandidates[i]->Weighting > moveCandidates[j]->Weighting) {

				Tile * temp = moveCandidates[i];
				moveCandidates[i] = moveCandidates[j];
				moveCandidates[j] = temp;
			}
		}
	}

	//reset all the tiles weightings to avoid pathfinding issues
	for (int i = 0; i < (int)moveCandidates.size(); i++)
		moveCandidates[i]->Weighting = 0;

	return moveCandidates;
}

Tile* Patrol::pickBestLOSTileFor(AICharacter * character) {

	vector<Tile*> moveCandidates = getBestLOSCandidatesFor(character);

	if ((int)_recentTiles.size() >= _maxRecent)
		//erase the oldest
		_recentTiles.erase(_recentTiles.begin() + 0);

	//add the latest tile to the recent list
	_recentTiles.push_back(moveCandidates[0]);

	//choose the best candidate to move to
	return moveCandidates[0];
}

//Cancels the patrol
void Patrol::cancel() {

	PatrolMove->cancel();
	State = kCancelling;
}

void Patrol::hardCancel() {

	PatrolMove->hardCancel();
	delete PatrolMove;

	Action::hardCancel();
}

//Patrol actions can be interuppted by interuppting the move action
bool Patrol::canInterrupt() {

	return true;
}

//Patrol actions cannot do anything else at the same time
bool Patrol::canDoBoth(Action * other) {

	return false;
}

//Action for when an enemy has been spotted
void Patrol::finished() {

	Action::finished();
}

bool Patrol::isSameKind(Action * other) {

	if (Patrol * derived = dynamic_cast<Patrol*>(other))
		return true;

	return false;
}

bool Patrol::shouldGiveWayTo(Action * other) {

	//if they are of the same type..
	if (isSameKind(other) == true) {

		return false;
	}

	//if not the same type, prioritise the action with the bigger priority
	else if (other->Priority >= Priority)
		return true;

	return false;
}

std::string Patrol::toString() {

	return "Patrolling";
}