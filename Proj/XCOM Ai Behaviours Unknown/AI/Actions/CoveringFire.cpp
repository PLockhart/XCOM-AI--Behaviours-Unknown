#include "CoveringFire.h"

#include <algorithm>

#include "../AICharacter.h"
#include "../../Characters and Weapons/BaseWeapon.h"
#include "../Team.h"
#include "SuppressAction.h"
#include "../../Containers/DataContainers.h"

CoveringFire::CoveringFire(AICharacter * actor, int priority)
: Overwatch(actor, priority) {

}


//Chooses the most easily targetable enemy to shoot at
AICharacter* CoveringFire::chooseTarget() {

	//if the weapon cannot suppress, just do a normal target assignment
	if (ActingCharacter->Weapon->canSuppress() == false)
		return Overwatch::chooseTarget();

	//otherwise, find targets that are attacking our team mates and suppress them
	vector<AICharacter*> friendAttackers;
	vector<AICharacter*> teammates = ActingCharacter->ParentTeam->getTeammatesOf(ActingCharacter);

	for (int i = 0; i < (int)teammates.size(); i++) {

		vector<AttackData> mateAttackers = teammates[i]->getAttackers();

		//loop through all of the attackers and add them to all the attackers
		for (int j = 0; j < (int)mateAttackers.size(); j++) {

			if (std::find(friendAttackers.begin(), friendAttackers.end(), mateAttackers[j].Attacker) == friendAttackers.end())
				friendAttackers.push_back(mateAttackers[j].Attacker);
		}
	}

	//if there are no attackers, just do a normal fire on visible enemies
	if (friendAttackers.size() == 0)
		return Overwatch::chooseTarget();

	//otherwise, pick the most aggressive enemy and suppress them
	AICharacter * bestTarget = friendAttackers[0];

	for (int i = 1; i <  (int)friendAttackers.size(); i++) {

		if (bestTarget->Aggression < friendAttackers[i]->Aggression)
			bestTarget = friendAttackers[i];
	}

	return bestTarget;
}

//suppreses the target
void CoveringFire::engageTarget(AICharacter * target) {

	_coveringAction = new SuppressAction(ActingCharacter, target, Priority);
}

std::string CoveringFire::toString() {

	return "Covering Fire";
}