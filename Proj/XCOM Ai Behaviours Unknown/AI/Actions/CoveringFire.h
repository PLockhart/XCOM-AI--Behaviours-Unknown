#ifndef CoveringFireH
#define CoveringFireH

#include "Overwatch.h"

class CoveringFire : public Overwatch {

	//Methods
	//===================================================
public:
	CoveringFire::CoveringFire(AICharacter * actor, int priority);

	std::string toString();

protected:
	AICharacter* chooseTarget();
	void engageTarget(AICharacter * target);
};

#endif