#include "AttackAction.h"

#include "../AICharacter.h"
#include "../../Characters and Weapons/BaseWeapon.h"
#include "../../Containers/DataContainers.h"
#include "../Raycast.h"

#include <iostream>

//Creates a new attack action with;
//the character who is shooting
//the character who is being shot at
//the level we are shooting in
AttackAction::AttackAction(AICharacter * actor, AICharacter * target, int priority)
	: Action(priority) {

	ActingCharacter = actor;
	Target = target;
}

void AttackAction::setup() {

	_isSetupFin = true;
	registerAttack();
}

void AttackAction::registerAttack() {

	Target->registerAttack(AttackData(ActingCharacter, kBasic));
}

//Returns true if the weapon is ready to fire and is trained on the enemy
bool AttackAction::isWeaponReady() {

	//only shoot if...
	//we have line of sight
	if (Raycast::castRay(ActingCharacter->CurrentTile, Target->CurrentTile) == true &&
		//the weapon has ammo
		ActingCharacter->Weapon->canShoot() == true &&
		//the weapon is in range of target
		ActingCharacter->Weapon->isInRange(Target->Position) == true)
		return true;

	return false;
}

//Performs the attack action
void AttackAction::performAttack() {

	ActingCharacter->Weapon->shootAt(Target);
}

//Shoots at the target while it still can
void AttackAction::act(float dt, AICharacter * sender) {

	Action::act(dt, sender);

	if (Target->IsDead == true)
		finished();

	//work out the direction we have to face
	Vector3 dirVector = Target->Position - ActingCharacter->Position;

	//resolve vector into an angle, where 0 is facing up n degrees
	float targetRot = ((atan2(dirVector.y, dirVector.x) * 180) / 3.1415926) + 90;
	Rotations::clampDegreeRotation(targetRot);

	if (isWeaponReady() == true) {

		//Only allow the movement along the direction vector if they are facing the general direction
		if (Rotations::rotationsSimilair(ActingCharacter->Rotation, (int)targetRot, ActingCharacter->RotSpeed) == true) {

			performAttack();
			ActingCharacter->faceTowards(Target->Position);
		}

		//else rotate to face them
		else {

			//rotate the charcter to face the target
			ActingCharacter->rotateBy(Rotations::RotDir(ActingCharacter->Rotation, targetRot) * ActingCharacter->RotSpeed);
		}
	}
	//else if the weapon is not ready then stop the action
	else
		finished();
}

//cancels shooting at the target
bool AttackAction::canInterrupt() {

	return true;
}

//Cant do anything while attacking
bool AttackAction::canDoBoth(Action * other) {

	return false;
}

//No shutdown routine for attacking, so just finish the attack action
void AttackAction::cancel() {

	Action::cancel();
	finished();
}

void AttackAction::hardCancel() {

	//the normal cancel of an attack action is immediate, so just call it
	cancel();
}

//Set the tile to be occupied then call the super finished method
void AttackAction::finished() {

	Target->unRegisterAttacker(ActingCharacter);
	Action::finished();
}

bool AttackAction::isSameKind(Action * other) {

	if (AttackAction * derived = dynamic_cast<AttackAction*>(other))
		return true;

	return false;
}

bool AttackAction::shouldGiveWayTo(Action * other) {

	//if they are of the same type..
	if (isSameKind(other) == true) {

		AttackAction * otherAtt = dynamic_cast<AttackAction*>(other);

		//don't bother switching actions if we have the same target
		if (otherAtt->Target == Target)
			false;

		else if (other->Priority >= Priority)
			return true;

		return false;
	}

	//if not the same type, prioritise the action with the bigger priority
	else if (other->Priority >= Priority)
		return true;

	return false;
}

std::string AttackAction::toString() {

	return "Attacking";
}