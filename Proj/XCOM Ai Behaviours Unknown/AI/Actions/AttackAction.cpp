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

void AttackAction::Setup() {

	_isSetupFin = true;
	RegisterAttack();
}

void AttackAction::RegisterAttack() {

	Target->RegisterAttack(AttackData(ActingCharacter, kBasic));
}

//Returns true if the weapon is ready to fire and is trained on the enemy
bool AttackAction::WeaponReady() {

	//only shoot if...
	//we have line of sight
	if (Raycast::CastRay(ActingCharacter->CurrentTile, Target->CurrentTile) == true &&
		//the weapon has ammo
		ActingCharacter->Weapon->CanShoot() == true &&
		//the weapon is in range of target
		ActingCharacter->Weapon->IsInRange(Target->Position) == true)
		return true;

	return false;
}

//Performs the attack action
void AttackAction::PerformAttack() {

	ActingCharacter->Weapon->ShootAt(Target);
}

//Shoots at the target while it still can
void AttackAction::Act(float dt, AICharacter * sender) {

	Action::Act(dt, sender);

	if (Target->IsDead == true)
		Finished();

	//work out the direction we have to face
	Vector3 dirVector = Target->Position - ActingCharacter->Position;

	//resolve vector into an angle, where 0 is facing up n degrees
	float targetRot = ((atan2(dirVector.y, dirVector.x) * 180) / 3.1415926) + 90;
	Rotations::ClampDegreeRotation(targetRot);

	if (WeaponReady() == true) {

		//Only allow the movement along the direction vector if they are facing the general direction
		if (Rotations::RotationsSimilair(ActingCharacter->Rotation, (int)targetRot, ActingCharacter->RotSpeed) == true) {

			PerformAttack();
			ActingCharacter->FaceTowards(Target->Position);
		}

		//else rotate to face them
		else {

			//rotate the charcter to face the target
			ActingCharacter->RotateBy(Rotations::RotDir(ActingCharacter->Rotation, targetRot) * ActingCharacter->RotSpeed);
		}
	}
	//else if the weapon is not ready then stop the action
	else
		Finished();
}

//cancels shooting at the target
bool AttackAction::CanInterrupt() {

	return true;
}

//Cant do anything while attacking
bool AttackAction::CanDoBoth(Action * other) {

	return false;
}

//No shutdown routine for attacking, so just finish the attack action
void AttackAction::Cancel() {

	Action::Cancel();
	Finished();
}

void AttackAction::HardCancel() {

	//the normal cancel of an attack action is immediate, so just call it
	Cancel();
}

//Set the tile to be occupied then call the super finished method
void AttackAction::Finished() {

	Target->UnRegisterAttacker(ActingCharacter);
	Action::Finished();
}

bool AttackAction::IsSameKind(Action * other) {

	if (AttackAction * derived = dynamic_cast<AttackAction*>(other))
		return true;

	return false;
}

bool AttackAction::ShouldGiveWayTo(Action * other) {

	//if they are of the same type..
	if (IsSameKind(other) == true) {

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

std::string AttackAction::ToString() {

	return "Attacking";
}