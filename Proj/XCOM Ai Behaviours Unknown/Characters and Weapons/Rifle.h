#ifndef RifleH
#define RifleH

#include "BaseWeapon.h"

class Rifle : public BaseWeapon {

public:
	Rifle::Rifle();
	std::string ToString();

	bool CanSuppress();
	void ModifyPlayerStats(AICharacter * character);
};

#endif
