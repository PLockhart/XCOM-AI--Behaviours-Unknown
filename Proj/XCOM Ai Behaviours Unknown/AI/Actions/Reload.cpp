#include "Reload.h"

#include "../AICharacter.h"
#include "../../Characters and Weapons/BaseWeapon.h"

Reload::Reload(AICharacter * actor, int priority)
: Action(priority) {

	_hasStarted = false;
	ActingCharacter = actor;
}

void Reload::setup() {

	_isSetupFin = true;
}

void Reload::act(float dt, AICharacter * sender) {
	
	Action::act(dt, sender);

	//reload the weapon if we haven't started doing it yet
	if  (_hasStarted == false) {

		_hasStarted = true;
		ActingCharacter->Weapon->reload();
	}

	//check for the reloading to be finished
	if (ActingCharacter->Weapon->IsReloading == false)
		finished();
}

//reloading actions cannot be cancelled
bool Reload::canInterrupt() {

	return false;
}

void Reload::cancel() {

	//cannot be cancelled
}

bool Reload::canDoBoth(Action * other) {

	return false;
}

bool Reload::isSameKind(Action * other) {

	if (Reload * derived = dynamic_cast<Reload*>(other))
		return true;

	return false;
}

bool Reload::shouldGiveWayTo(Action * other) {

	//if they are of the same type..
	if (isSameKind(other) == true) {

		return false;
	}

	//if not the same type, prioritise the action with the bigger priority
	else if (other->Priority >= Priority)
		return true;

	return false;
}

std::string Reload::toString() {

	return "Reloading";
}