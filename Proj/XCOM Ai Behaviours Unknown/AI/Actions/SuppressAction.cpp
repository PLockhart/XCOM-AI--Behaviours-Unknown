#include "SuppressAction.h"

#include "../AICharacter.h"
#include "../Team.h"
#include "../../Characters and Weapons/BaseWeapon.h"
#include "../../Containers/DataContainers.h"

SuppressAction::SuppressAction(AICharacter * actor, AICharacter * target, int priority)
:AttackAction(actor, target, priority) {

}

void SuppressAction::registerAttack() {

	Target->registerAttack(AttackData(ActingCharacter, kSuppressing));
}

//Suppresses the target
void SuppressAction::performAttack() {

	ActingCharacter->Weapon->suppressTarget(Target);
}

bool SuppressAction::isSameKind(Action * other) {

	if (SuppressAction * derived = dynamic_cast<SuppressAction*>(other))
		return true;

	return false;
}

bool SuppressAction::shouldGiveWayTo(Action * other) {

	//if we have no one to suppress for, then give way to whatever action
	if (ActingCharacter->ParentTeam->getTeammatesOf(ActingCharacter).size() == 0)
		return true;

	//otherwise, do normal action handling
	else
		return AttackAction::shouldGiveWayTo(other);
}

std::string SuppressAction::toString() {

	return "Suppressing";
}