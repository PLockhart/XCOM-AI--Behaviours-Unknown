#include "Reload.h"

#include "../AICharacter.h"
#include "../../Characters and Weapons/BaseWeapon.h"

Reload::Reload(AICharacter * actor, int priority)
: Action(priority) {

	_hasStarted = false;
	ActingCharacter = actor;
}

void Reload::Setup() {

	_isSetupFin = true;
}

void Reload::Act(float dt, AICharacter * sender) {
	
	Action::Act(dt, sender);

	//reload the weapon if we haven't started doing it yet
	if  (_hasStarted == false) {

		_hasStarted = true;
		ActingCharacter->Weapon->Reload();
	}

	//check for the reloading to be finished
	if (ActingCharacter->Weapon->IsReloading == false)
		Finished();
}

//reloading actions cannot be cancelled
bool Reload::CanInterrupt() {

	return false;
}

void Reload::Cancel() {

	//cannot be cancelled
}

bool Reload::CanDoBoth(Action * other) {

	return false;
}

bool Reload::IsSameKind(Action * other) {

	if (Reload * derived = dynamic_cast<Reload*>(other))
		return true;

	return false;
}

bool Reload::ShouldGiveWayTo(Action * other) {

	//if they are of the same type..
	if (IsSameKind(other) == true) {

		return false;
	}

	//if not the same type, prioritise the action with the bigger priority
	else if (other->Priority >= Priority)
		return true;

	return false;
}

std::string Reload::ToString() {

	return "Reloading";
}