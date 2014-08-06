#include "Shotgun.h"

#include "../AI/AICharacter.h"

Shotgun::Shotgun() 
	: BaseWeapon() {

		FireRate = 1;
		ReloadTime = 2;
		Range = 250;
		Accuracy = 115;
		AccuracyDrop = 1.0f;
		ClipSize = _shotsInClip = 4;
		BulletsPerShot = 8;
		CoverPenalty = 1.2f;
}

//Override of base weapon. Shotguns cannot suppress
bool Shotgun::CanSuppress() {

	return false;
}

void Shotgun::ModifyPlayerStats(AICharacter * character) {

	//a character weilding a shotgun is more aggressive and slightly more bold
	character->Aggression = min(character->Aggression + 4, 10);
	character->Boldness = min(character->Boldness + 2, 10);
}

std::string Shotgun::ToString() {

	return "Shotgun";
}