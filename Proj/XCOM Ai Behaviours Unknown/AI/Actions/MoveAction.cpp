#include "MoveAction.h"

#include "../AICharacter.h"
#include "../PathFinding.h"

MoveAction::MoveAction(int priority)
:Action(priority) {}

//Creates a new movement action with;
//the character that will be performing the action, 
//the tile that the character must move to
MoveAction::MoveAction(AICharacter * character, Tile * destination, int priority)
	: Action(priority) {

	ActingCharacter = character;
	_originalDestination = destination;
}

void MoveAction::Setup() {

	_isSetupFin = true;

	//the character is moving from their current tile, so don't mark it as occupied
	ActingCharacter->CurrentTile->IsOccupied = false;

	//try and work out a path to the destination
	bool _pathCalculated;
	_pathCalculated = GetPathwayData(_moveTiles, ActingCharacter->CurrentTile, _originalDestination);

	//if we coulld plot a path, set the characters destination to the first index
	if (_pathCalculated == true) {

		Destination = _originalDestination->Position;
		ActingCharacter->DestinationTile = _moveTiles[0];
	}

	//otherwise cancel the movement
	else {

		Cancel();
	}
}

//Gets the pathway data from a start to a finish point.
//Returns the outcome of the procedure, true if a path was found
bool MoveAction::GetPathwayData(vector<Tile*> &pathway, Tile * start, Tile * destination) {

	//normal movement, so nothing special
	return PathFinding::GetPathway(pathway, start, destination);
}

//Moves the player's vector along the move positions
void MoveAction::Act(float dt, AICharacter * sender) {

	Action::Act(dt, sender);

	//validation check
	if (abs(ActingCharacter->DestinationTile->X - ActingCharacter->CurrentTile->X) > 1 ||
		abs(ActingCharacter->DestinationTile->Y - ActingCharacter->CurrentTile->Y) > 1) {

		Cancel();
		return;
	}

	//if we have tiles to move to keep cycling through them moving to each one in turn
	if (_moveTiles.size() != 0) {

		//if the overall destination is occupied by another character then cancel the action
		if (_moveTiles.back()->IsOccupied == true) {

			Cancel();
			return;
		}

		//work out the move vector to the next point
		Vector3 nextPoint = (_moveTiles.front())->Position;
		//work out the direction we have to face
		Vector3 moveVector = nextPoint - ActingCharacter->Position;

		//resolve vector into an angle, where 0 is facing up n degrees
		float targetRot = ((atan2(moveVector.y, moveVector.x) * 180) / 3.1415926) + 90;
		Rotations::ClampDegreeRotation(targetRot);

		//Only allow the movement along the direction vector if they are facing the general direction
		if (Rotations::RotationsSimilair(ActingCharacter->Rotation, targetRot, ActingCharacter->RotSpeed) == false) {

			ActingCharacter->RotateBy(Rotations::RotDir(ActingCharacter->Rotation, targetRot) * ActingCharacter->RotSpeed);
		}
		//else the player is facing in the right direction and ready to move
		else {

			moveVector.normalise();

			//move the player along its move vector
			ActingCharacter->MoveBy((moveVector * ActingCharacter->Speed * ActingCharacter->MoveModifier));

			//check to see if the character has moved on to the destination tile
			if (ActingCharacter->CurrentTile == _moveTiles.front()) {

				//remove that point from the movePositions list
				_moveTiles.erase(_moveTiles.begin());

				//move on to the next tile in the list while we stil have a series of tiles to move through
				if (_moveTiles.size() != 0)
					ActingCharacter->DestinationTile = _moveTiles[0];
			}
		}
	}

	//if the character has no destination other than the tile they are on, then move them to the center of that tile
	else if (ActingCharacter->CurrentTile == ActingCharacter->DestinationTile) {

		//if the destination has been taken then cancel the move
		if (ActingCharacter->DestinationTile->IsOccupied == true) {

			Cancel();
			return;
		}

		//work out the move vector to the next point
		Vector3 nextPoint = ActingCharacter->CurrentTile->Position;
		Vector3 moveVector = nextPoint - ActingCharacter->Position;
		moveVector.normalise();

		//if the character is basically on top of the tile's center position, make it so
		if (ActingCharacter->Position.distance(nextPoint) < ActingCharacter->Speed * ActingCharacter->MoveModifier)
			ActingCharacter->MoveBy(ActingCharacter->CurrentTile->Position - ActingCharacter->Position);

		//move the character along its move vector
		else
			ActingCharacter->MoveBy(moveVector * ActingCharacter->Speed * ActingCharacter->MoveModifier);
	}

	//if the cahracter has no more tiles ot move though and they are in the center of their current tile, then finish the action
	if (ActingCharacter->Position == ActingCharacter->CurrentTile->Position &&
		_moveTiles.size() == 0) 
		Finished();
}

//Cancels the movement and moves the character to the nearest valid tile
void MoveAction::Cancel() {

	//don't bother redoing the cancel if we are already cancelled or have already finished
	if (State == kCancelling || Action::IsActionComplete() == true)
		return;

	State = kCancelling;
	_moveTiles.clear();

	//with this uncommented, the charcter must always move to the center position before the action can finish
	//just move the character to the centre of the tile they were on, if not occupied
	//if (ActingCharacter->CurrentTile->IsOccupied == false)
	//	ActingCharacter->DestinationTile = ActingCharacter->CurrentTile;

	//with this uncommented, the character can end in an odd position in the tile
	if (ActingCharacter->CurrentTile->IsOccupied == false) {

		ActingCharacter->DestinationTile = ActingCharacter->CurrentTile;
		Finished();
	}

	//if we can't immediately find a fallback, search out to the neighbouring tiles
	else {

		Tile *closest = GetIdealFallback();

		//path find to the worked out nearest tile
		GetPathwayData(_moveTiles, ActingCharacter->CurrentTile, closest);
		ActingCharacter->DestinationTile = _moveTiles[0];
	}
}

//Gets the closest valid tile we can move to, to stop moving
Tile* MoveAction::GetIdealFallback() {

	bool foundDestination = false;
	int depth = 1;
	Tile *closest;

	while (foundDestination == false) {

		//flood map and find a random neighbour to go to
		vector<Tile*> neighbours;
		PathFinding::FloodMap(neighbours, ActingCharacter->CurrentTile, depth);

		for (int i = 0; i < (int)neighbours.size(); i++) {
			//remove any neighbours that are occupied
			if (neighbours[i]->IsOccupied == true) {

				neighbours.erase(neighbours.begin() + i);
				i--;
			}
		}

		//if there are possible tiles to move to, work out the nearest
		if ((int)neighbours.size() > 1) {

			//pick the closest valid neighbour
			closest = neighbours[0];
			for (int i = 0; i < (int)neighbours.size(); i++) {

				//if we found a closer tile set it as the next closest
				if (ActingCharacter->Position.distance(closest->Position) > ActingCharacter->Position.distance(neighbours[i]->Position))
					closest = neighbours[i];
			}

			foundDestination = true;
			break;
		}

		//increase the depth in case we couldn't find neighbours at the current depth
		depth++;
	}

	return closest;
}

//cancels moving to the target
bool MoveAction::CanInterrupt() {

	return true;
}

//Cant do anything while moving
bool MoveAction::CanDoBoth(Action * other) {

	return false;
}

//Set the tile to be occupied then call the super finished method
void MoveAction::Finished() {

	ActingCharacter->CurrentTile->IsOccupied = true;
	Action::Finished();
}

bool MoveAction::IsSameKind(Action * other) {

	if (MoveAction * derived = dynamic_cast<MoveAction*>(other))
		return true;

	return false;
}

bool MoveAction::ShouldGiveWayTo(Action * other) {

	//if they are of the same type..
	if (IsSameKind(other) == true) {

		if (other->Priority >= Priority)
			return true;

		return false;
	}

	//if not the same type, prioritise the action with the bigger priority
	else if (other->Priority >= Priority)
		return true;

	return false;
}

std::string MoveAction::ToString() {

	return "Moving";
}