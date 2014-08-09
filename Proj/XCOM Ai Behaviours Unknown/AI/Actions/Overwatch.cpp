#include "Overwatch.h"

#include "../AICharacter.h"
#include "../../Characters and Weapons/BaseWeapon.h"

Overwatch::Overwatch(AICharacter * actor, int priority)
: Action(priority) {

	ActingCharacter = actor;
	_hasAction = false;
}

void Overwatch::setup() {

	_isSetupFin = true;
}

void Overwatch::act(float dt, AICharacter * sender) {

	Action::act(dt, sender);

	//if the overwatch has finished cancelling, finish it
	if (State == kCancelling && _coveringAction->isActionComplete()) {

		delete _coveringAction;
		_hasAction = false;

		finished();

		return;
	}

	//if we aren't attacking, set up an attack action if enemies are visible
	if (ActingCharacter->VisibleEnemies.size() > 0) {

		AICharacter * target = chooseTarget();

		if (_hasAction == false) {

			engageTarget(target);
		}

		else {

			//if we now have a better target to engage, cancel the covering action
			if (_coveringAction->Target != target) {

				_coveringAction->cancel();
			}
		}
	}

	if (_hasAction == true) {

		//perform the covering action if we have an action
		_coveringAction->act(dt, sender);

		//if the covering action is complete, remove it from memory
		if (_coveringAction->isActionComplete() == true) {

			delete _coveringAction;
			_hasAction = false;
		}
	}
}

//Chooses the most easily targetable enemy to shoot at
AICharacter* Overwatch::chooseTarget() {

	vector<AICharacter*> visibleEnemies = ActingCharacter->VisibleEnemies;

	AICharacter * bestTarget = visibleEnemies[0];
	float bestAccuracy = ActingCharacter->Weapon->getAccuracyToCharIncCover(bestTarget);

	//loop through all the other enemies and find the most easily targetable
	for (int i = 1; i < (int)visibleEnemies.size(); i++) {

		float loopedAccuracy = ActingCharacter->Weapon->getAccuracyToCharIncCover(visibleEnemies[i]);

		if (loopedAccuracy > bestAccuracy) {

			bestAccuracy = loopedAccuracy;
			bestTarget = visibleEnemies[i];
		}
	}

	return bestTarget;
}

//engages the target with a shooting action
void Overwatch::engageTarget(AICharacter * target) {

	_coveringAction = new AttackAction(ActingCharacter, target, Priority);
}

//overwatch actions can be cancelled
bool Overwatch::canInterrupt() {

	return true;
}

void Overwatch::cancel() {

	Action::cancel();
	_coveringAction->cancel();
}

bool Overwatch::canDoBoth(Action * other) {

	return false;
}

bool Overwatch::isSameKind(Action * other) {

	if (Overwatch * derived = dynamic_cast<Overwatch*>(other))
		return true;

	return false;
}

bool Overwatch::shouldGiveWayTo(Action * other) {

	//if they are of the same type..
	if (isSameKind(other) == true) {

		return false;
	}

	//if not the same type, prioritise the action with the bigger priority
	else if (other->Priority >= Priority)
		return true;

	return false;
}

std::string Overwatch::toString() {

	return "Overwatching";
}