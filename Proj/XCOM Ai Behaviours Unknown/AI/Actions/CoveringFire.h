#ifndef CoveringFireH
#define CoveringFireH

#include "Overwatch.h"

class CoveringFire : public Overwatch {

	//Methods
	//===================================================
public:
	CoveringFire::CoveringFire(AICharacter * actor, int priority);

	std::string ToString();

protected:
	AICharacter* ChooseTarget();
	void EngageTarget(AICharacter * target);
};

#endif