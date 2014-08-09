#ifndef RifleH
#define RifleH

#include "BaseWeapon.h"

class Rifle : public BaseWeapon {

public:
	Rifle::Rifle();
	std::string toString();

	bool canSuppress();
	void modifyPlayerStats(AICharacter * character);
};

#endif
