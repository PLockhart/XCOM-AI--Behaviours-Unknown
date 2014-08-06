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
	if (char1->Weapon->CanSuppress() == true) {

		ActingCharacter = char1;
		CoveringCharacter = char2;
	}

	else {

		ActingCharacter = char2;
		CoveringCharacter = char1;
	}

	IsPatrollerInPosition = false;
}

void BoundingOverwatch::Act(float dt, AICharacter * sender) {

	//if the sender of this action is the character who is moving, run the patrol action
	if (sender == ActingCharacter)
		Patrol::Act(dt, sender);
}

//event for when the mover has finished their movement to their destination
void BoundingOverwatch::MoverFinished() {

	//swap the covering unit and the moving unit
	AICharacter * temp = ActingCharacter;
	ActingCharacter = CoveringCharacter;
	CoveringCharacter = temp;

	//delete the old patrol move
	delete PatrolMove;

	if (State != kCancelling) {

		//get the best tile that the new covering character can watch over
		Tile * best = PickBestLOSTileFor(CoveringCharacter);

		//and get the character that was previously covering to move to that new position
		PatrolMove = new MoveAction(ActingCharacter, best, Priority);
	}

	else
		Finished();
}

//gets the best line of sight tiles in order of best, where tiles are only valid if they can be covered by the character
vector<Tile*> BoundingOverwatch::GetBestLOSCandidatesFor(AICharacter * character) {

	vector<Tile*> moveCandidates = Patrol::GetBestLOSCandidatesFor(character);

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
void BoundingOverwatch::Cancel() {

	Patrol::Cancel();

	//allow the other character to immediatly perform other actions
	CoveringCharacter->HasAction = false;
}

void BoundingOverwatch::HardCancel() {

	if (_isSetupFin == true)
		Patrol::HardCancel();

	CoveringCharacter->HasAction = false;
}

//Action for when an enemy has been spotted
void BoundingOverwatch::Finished() {

	Patrol::Finished();
}

void BoundingOverwatch::Deleted() {

	ActingCharacter->HasAction = false;
	CoveringCharacter->HasAction = false;
}

bool BoundingOverwatch::IsSameKind(Action * other) {

	if (BoundingOverwatch * derived = dynamic_cast<BoundingOverwatch*>(other))
		return true;

	return false;
}

bool BoundingOverwatch::ShouldGiveWayTo(Action * other) {

	//if they are of the same type..
	if (IsSameKind(other) == true) {

		return false;
	}

	//if not the same type, prioritise the action with the bigger priority
	else if (other->Priority >= Priority)
		return true;

	return false;
}

std::string BoundingOverwatch::ToString() {

	return "Bounding Overwatch";
}