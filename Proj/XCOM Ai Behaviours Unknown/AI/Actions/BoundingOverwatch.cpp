#include "BoundingOverwatch.h"

#include <algorithm>

#include "../PathFinding.h"
#include "../../Containers/DataContainers.h"
#include "../AICharacter.h"
#include "../Team.h"
#include "../../Level/Tile.h"
#include "../../Characters and Weapons/BaseWeapon.h"

//A bounding overwatch where 1 character covers while the other character moves
BoundingOverwatch::BoundingOverwatch(AICharacter * char1, AICharacter * char2, int priority)
: Patrol(char1, priority) {

	//if one of the characters can suppress, make them the coverer
	if (char1->Weapon->canSuppress() == true) {

		ActingCharacter = char1;
		CoveringCharacter = char2;
	}

	else {

		ActingCharacter = char2;
		CoveringCharacter = char1;
	}

	IsPatrollerInPosition = false;
}

void BoundingOverwatch::act(float dt, AICharacter * sender) {

	//if the sender of this action is the character who is moving, run the patrol action
	if (sender == ActingCharacter)
		Patrol::act(dt, sender);
}

//event for when the mover has finished their movement to their destination
void BoundingOverwatch::moverFinished() {

	//swap the covering unit and the moving unit
	AICharacter * temp = ActingCharacter;
	ActingCharacter = CoveringCharacter;
	CoveringCharacter = temp;

	//delete the old patrol move
	delete PatrolMove;

	if (State != kCancelling) {

		//get the best tile that the new covering character can watch over
		Tile * best = pickBestLOSTileFor(CoveringCharacter);

		//and get the character that was previously covering to move to that new position
		PatrolMove = new MoveAction(ActingCharacter, best, Priority);
	}

	else
		finished();
}

//gets the best line of sight tiles in order of best, where tiles are only valid if they can be covered by the character
vector<Tile*> BoundingOverwatch::getBestLOSCandidatesFor(AICharacter * character) {

	vector<Tile*> moveCandidates = Patrol::getBestLOSCandidatesFor(character);

	//elimate all move candidates that are not covered by the character
	for (int i = 0; i < (int)moveCandidates.size(); i++) {

		bool isCovered = false;

		//if not in the covering units infulenced tiles, remove it from the move candidates
		for (int j = 0 ; j < (int)character->InfulencedTiles.size(); j++) {

			if (moveCandidates[i] == character->InfulencedTiles[j].TheTile) {

				isCovered = true;
				break;
			}
		}

		if (isCovered == false) {

			moveCandidates.erase(moveCandidates.begin() + i);
			i--;
		}
	}

	return moveCandidates;
}


//Cancels the patrol
void BoundingOverwatch::cancel() {

	Patrol::cancel();

	//allow the other character to immediatly perform other actions
	CoveringCharacter->HasAction = false;
}

void BoundingOverwatch::hardCancel() {

	if (_isSetupFin == true)
		Patrol::hardCancel();

	CoveringCharacter->HasAction = false;
}

//Action for when an enemy has been spotted
void BoundingOverwatch::finished() {

	Patrol::finished();
}

void BoundingOverwatch::deleted() {

	ActingCharacter->HasAction = false;
	CoveringCharacter->HasAction = false;
}

bool BoundingOverwatch::isSameKind(Action * other) {

	if (BoundingOverwatch * derived = dynamic_cast<BoundingOverwatch*>(other))
		return true;

	return false;
}

bool BoundingOverwatch::shouldGiveWayTo(Action * other) {

	//if they are of the same type..
	if (isSameKind(other) == true) {

		return false;
	}

	//if not the same type, prioritise the action with the bigger priority
	else if (other->Priority >= Priority)
		return true;

	return false;
}

std::string BoundingOverwatch::toString() {

	return "Bounding Overwatch";
}