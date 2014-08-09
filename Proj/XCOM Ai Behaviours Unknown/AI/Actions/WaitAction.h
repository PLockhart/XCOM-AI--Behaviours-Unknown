#ifndef WaitActionH
#define WaitActionH

#include "../Action.h"

class WaitAction : public Action {

	//Methods
	//===================================================
public:
	WaitAction::WaitAction(AICharacter * actor, int priority);
	void act(float dt, AICharacter * sender);
	
	bool canInterrupt();
	bool canDoBoth(Action * other);
	void cancel();

	bool isSameKind(Action * other);
	bool shouldGiveWayTo(Action *other);

	std::string toString();

protected:
	void setup();
};

#endif