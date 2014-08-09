#include "AmmoCheck.h"
#include "../DecisionTree.h"
#include "../AICharacter.h"
#include "../../Characters and Weapons/BaseWeapon.h"

//Checks to see if the user has a minimum percentage of ammo in their weapon
HasMinAmmo::HasMinAmmo(DecisionTree * tree, float minPercent)
: Decision(tree) {

	_minPercent = minPercent;
}

//checks to see if the actor has an acceptable amount of ammo
Action* HasMinAmmo::run() {

	if (Tree->Character->Weapon->getAmmoRatio() > _minPercent / 100) {

		return TrueBranch->run();
	}

	Tree->log("Need ammo");
	return FalseBranch->run();
}