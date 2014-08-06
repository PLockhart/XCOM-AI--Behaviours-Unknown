#include "SuppressAction.h"

#include "../AICharacter.h"
#include "../Team.h"
#include "../../Characters and Weapons/BaseWeapon.h"
#include "../../Containers/DataContainers.h"

SuppressAction::SuppressAction(AICharacter * actor, AICharacter * target, int priority)
:AttackAction(actor, target, priority) {

}

void SuppressAction::RegisterAttack() {

	Target->RegisterAttack(AttackData(ActingCharacter, kSuppressing));
}

//Suppresses the target
void SuppressAction::PerformAttack() {

	ActingCharacter->Weapon->SuppressTarget(Target);
}

bool SuppressAction::IsSameKind(Action * other) {

	if (SuppressAction * derived = dynamic_cast<SuppressAction*>(other))
		return true;

	return false;
}

bool SuppressAction::ShouldGiveWayTo(Action * other) {

	//if we have no one to suppress for, then give way to whatever action
	if (ActingCharacter->ParentTeam->GetTeammatesOf(ActingCharacter).size() == 0)
		return true;

	//otherwise, do normal action handling
	else
		return AttackAction::ShouldGiveWayTo(other);
}

std::string SuppressAction::ToString() {

	return "Suppressing";
}