#include "Overwatch.h"

#include "../AICharacter.h"
#include "../../Characters and Weapons/BaseWeapon.h"

Overwatch::Overwatch(AICharacter * actor, int priority)
: Action(priority) {

	ActingCharacter = actor;
	_hasAction = false;
}

void Overwatch::Setup() {

	_isSetupFin = true;
}

void Overwatch::Act(float dt, AICharacter * sender) {

	Action::Act(dt, sender);

	//if the overwatch has finished cancelling, finish it
	if (State == kCancelling && _coveringAction->IsActionComplete()) {

		delete _coveringAction;
		_hasAction = false;

		Finished();

		return;
	}

	//if we aren't attacking, set up an attack action if enemies are visible
	if (ActingCharacter->VisibleEnemies.size() > 0) {

		AICharacter * target = ChooseTarget();

		if (_hasAction == false) {

			EngageTarget(target);
		}

		else {

			//if we now have a better target to engage, cancel the covering action
			if (_coveringAction->Target != target) {

				_coveringAction->Cancel();
			}
		}
	}

	if (_hasAction == true) {

		//perform the covering action if we have an action
		_coveringAction->Act(dt, sender);

		//if the covering action is complete, remove it from memory
		if (_coveringAction->IsActionComplete() == true) {

			delete _coveringAction;
			_hasAction = false;
		}
	}
}

//Chooses the most easily targetable enemy to shoot at
AICharacter* Overwatch::ChooseTarget() {

	vector<AICharacter*> visibleEnemies = ActingCharacter->VisibleEnemies;

	AICharacter * bestTarget = visibleEnemies[0];
	float bestAccuracy = ActingCharacter->Weapon->GetAccuracyToCharIncCover(bestTarget);

	//loop through all the other enemies and find the most easily targetable
	for (int i = 1; i < (int)visibleEnemies.size(); i++) {

		float loopedAccuracy = ActingCharacter->Weapon->GetAccuracyToCharIncCover(visibleEnemies[i]);

		if (loopedAccuracy > bestAccuracy) {

			bestAccuracy = loopedAccuracy;
			bestTarget = visibleEnemies[i];
		}
	}

	return bestTarget;
}

//engages the target with a shooting action
void Overwatch::EngageTarget(AICharacter * target) {

	_coveringAction = new AttackAction(ActingCharacter, target, Priority);
}

//overwatch actions can be cancelled
bool Overwatch::CanInterrupt() {

	return true;
}

void Overwatch::Cancel() {

	Action::Cancel();
	_coveringAction->Cancel();
}

bool Overwatch::CanDoBoth(Action * other) {

	return false;
}

bool Overwatch::IsSameKind(Action * other) {

	if (Overwatch * derived = dynamic_cast<Overwatch*>(other))
		return true;

	return false;
}

bool Overwatch::ShouldGiveWayTo(Action * other) {

	//if they are of the same type..
	if (IsSameKind(other) == true) {

		return false;
	}

	//if not the same type, prioritise the action with the bigger priority
	else if (other->Priority >= Priority)
		return true;

	return false;
}

std::string Overwatch::ToString() {

	return "Overwatching";
}