#include "Rifle.h"

#include "../AI/AICharacter.h"

Rifle::Rifle() 
	: BaseWeapon() {

		FireRate = 0.15f;
		ReloadTime = 2;
		Range = 900;
		Accuracy = 100;
		AccuracyDrop = 0.6f;
		ClipSize = _shotsInClip = 30;
		BulletsPerShot = 1;
		CoverPenalty = 2;
		SuppressPenality = 0.2f;
}

//Override of base weapon. Must have ammunition and not be reloading
bool Rifle::CanSuppress() {

	if (_shotsInClip >= 1 && IsReloading == false)
		return true;
	
	return false;
}

void Rifle::ModifyPlayerStats(AICharacter * character) {

	//a rifle doesn't add any traits to the character
}

std::string Rifle::ToString() {

	return "Rifle";
}