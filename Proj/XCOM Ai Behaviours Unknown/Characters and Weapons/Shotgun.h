#ifndef ShotgunH
#define ShotgunH

#include "BaseWeapon.h"

class Shotgun : public BaseWeapon {

public:
	Shotgun::Shotgun();
	std::string ToString();

	bool CanSuppress();
	void ModifyPlayerStats(AICharacter * character);
};

#endif