#include "Shotgun.h"

#include "../AI/AICharacter.h"

const int SHOTGUN_AGGRESSION_BONUS = 4;
const int SHOTFGUN_BOLDNESS_BONUS = 2;

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
bool Shotgun::canSuppress() {

	return false;
}

void Shotgun::modifyPlayerStats(AICharacter * character) {

	//a character weilding a shotgun is more aggressive and slightly more bold
	character->Aggression = min(character->Aggression + SHOTGUN_AGGRESSION_BONUS, 10);
	character->Boldness = min(character->Boldness + SHOTFGUN_BOLDNESS_BONUS, 10);
}

std::string Shotgun::toString() {

	return "Shotgun";
}