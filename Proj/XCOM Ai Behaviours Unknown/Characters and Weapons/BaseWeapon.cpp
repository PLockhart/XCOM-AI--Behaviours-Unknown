#include "BaseWeapon.h"

#include <iostream>
#include <random>

#include "../AI/AICharacter.h"

//Base init for any weapons with;
//the holder that will be using this gun
BaseWeapon::BaseWeapon() {

	FireRate = 0;
	ReloadTime = 0;
	IsReloading = false;
	Range = 0;
	Accuracy = 0;
	AccuracyDrop = 0;
	ClipSize = 0;
	BulletsPerShot = 0;
	SuppressPenality = 1;

	_shotsInClip = 0;
	_fireCooldownTime = 0;
	_reloadCooldownTime = 0;
	_weaponFired = false;
}

//Fires the weapon at the target
void BaseWeapon::ShootAt(AICharacter * target) {
	
	if (CanShoot() == true && IsInRange(target->Position) == true && _fireCooldownTime <= 0) {

		//reduce the clip size and reset the time until we can fire next
		_shotsInClip--;
		_fireCooldownTime = FireRate;

		_weaponFired = true;

		float calAccuracy = GetAccuracyToCharIncCover(target);

		if (rand() % 100 < calAccuracy) {

			//hit target, damage them for the amount of bullets we fire
			for (int i = 0; i < BulletsPerShot; i++) 
				target->DamageTaken(Holder);

			_targetPos = target->Position;
			_shotType = kHit;
		}
		//if they AI missed, create a random position near the target for the simulation line to go to
		else {

			_shotType = kMiss;
			_targetPos = target->Position + Vector3((rand() % 30) + - 30, (rand() % 30) + - 30, 0);
		}
	}
}

//Returns the accuracy of the weapon when firing to a certain position. Ignores cover
float BaseWeapon::GetAccuracyToTarget(Vector3 targetPos) {

	return GetAccuracyFromPlaceToTarget(Holder->Position, targetPos);
}

//Returns the accuracy of the weapon if it was at a given position, firing to a certain position. Ignores cover
float BaseWeapon::GetAccuracyFromPlaceToTarget(Vector3 startPos, Vector3 targetPos) {

	float distance = startPos.distance(targetPos);
	
	//return if we are out of range
	if (distance > Range)
		return 0;

	//work out accuracy to target, taking into account accuracy drop
	float accuracyDrop = (distance / Range * 100 * AccuracyDrop);
	float accuracyOverDist = Accuracy - accuracyDrop;

	//modify by the holders accuracy
	accuracyOverDist *= Holder->AccuracyModifier;

	//if we can shoot the target, return the accuracy
	if (accuracyOverDist > 0)
		return accuracyOverDist;

	else
		return 0;
}

//Returns the accuracy of the weapon when shooting at a specified target, taking into account cover
float BaseWeapon::GetAccuracyToCharIncCover(AICharacter * target) {

	//get the base accuracy to the position the target is at
	float acc = GetAccuracyToTarget(target->Position);

	//if the target is in cover from our attack, reduce the accuracy
	if (target->IsInCoverFromAttack(Holder->Position) == true)
			acc /= CoverPenalty;

	return acc;
}

//Flag for whether the weapon can shoot, where it either has no ammo or is currently being reloaded
bool BaseWeapon::CanShoot() {

	if (_shotsInClip >= 1 && IsReloading == false)	
		return true;
	
	return false;
}

//Checks to see if the weapon is in range of a target
bool BaseWeapon::IsInRange(Vector3 position) {

	if (GetAccuracyToTarget(position) > 0)
		return true;

	return false;
}

//Starts reloading the weapon if it is not already doing so
void BaseWeapon::Reload() {

	if (IsReloading == false) {

		IsReloading = true;
		_reloadCooldownTime = ReloadTime;
	}
}

//Supresses a target, reducing the weapons shots in clip and modifying the targets accuracy/move modifiers
void BaseWeapon::SuppressTarget(AICharacter * target) {

	//check if the weapon is capable of suppression, and is in range of target
	if (CanSuppress() == true && IsInRange(target->Position)) {

		//a suppression's effect on a unit is continous, but only reduce its cooldowns..
		//..and only draw within the cool down boundaries. This prevents a constant line effect and rapid ammo draining
		if (_fireCooldownTime <= 0) {

			_shotsInClip--;
			_fireCooldownTime = FireRate;

			_weaponFired = true;
			_shotType = kSuppress;
			_targetPos = target->Position;
		}

		//don't modify the targets modifiers if they are slower than what we'd make them by suppressing
		if (target->AccuracyModifier > SuppressPenality)
			target->AccuracyModifier = SuppressPenality;

		if (target->MoveModifier > SuppressPenality)
			target->MoveModifier = SuppressPenality;
	}
}

//Updates the weapon/cooldowns etc
void BaseWeapon::Update(float dt) {

	_weaponFired = false;

	//reduce cooldowns
	if (_fireCooldownTime >= 0)
		_fireCooldownTime -= dt;

	//if reloading, check for reload finished and replinish bullets
	if (IsReloading == true) {

		_reloadCooldownTime -= dt;

		//check if the reload time has passed
		if (_reloadCooldownTime <= 0) {

			_shotsInClip = ClipSize;
			IsReloading = false;
		}
	}

	//update the characters aggression depending on the amount of ammo left
	if (GetAmmoRatio() <= 0.25f) {

		if (_shotsInClip <= 0)
			Holder->Aggression = 0;
		else
			Holder->Aggression = max(Holder->Aggression - 2, 0);
	}
}

//returns the ratio between the number of shots in the gun compared to its max capcity
float BaseWeapon::GetAmmoRatio() {

	return _shotsInClip / (float)ClipSize;
}

//Draws any shot simulations that the weapon may have done
void BaseWeapon::Draw() {

	if (_weaponFired == true) {

		glPushMatrix();

		Vector3 holderPos = Holder->Position;

		//draw a green colour if the shot was sucessful
		switch (_shotType) {

		case kHit:
			glColor3f(0.0, 1.0, 0.0);	//green
			break;

		case kMiss:
			glColor3f(1.0, 0.0, 0.0);	//red
			break;

		case kSuppress:
			glColor3f(1.0, 1.0, 0.0);	//yellow
			break;
		}

		glLineWidth(5); 
		glBegin(GL_LINES);

		//draw from the holder's position to the target's position
		glVertex3f(holderPos.x, holderPos.y, 0.0);
		glVertex3f(_targetPos.x, _targetPos.y, 0);
		glEnd();

		//reset the colour
		glColor3f(1.0, 1.0, 1.0);

		glPopMatrix();
	}
}