#ifndef ShotgunH
#define ShotgunH

#include "BaseWeapon.h"

class Shotgun : public BaseWeapon {

public:
	Shotgun::Shotgun();
	std::string toString();

	bool canSuppress();
	void modifyPlayerStats(AICharacter * character);
};

#endif