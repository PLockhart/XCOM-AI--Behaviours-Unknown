#ifndef WaitActionH
#define WaitActionH

#include "../Action.h"

class WaitAction : public Action {

	//Methods
	//===================================================
public:
	WaitAction::WaitAction(AICharacter * actor, int priority);
	void Act(float dt, AICharacter * sender);
	
	bool CanInterrupt();
	bool CanDoBoth(Action * other);
	void Cancel();

	bool IsSameKind(Action * other);
	bool ShouldGiveWayTo(Action *other);

	std::string ToString();

protected:
	void Setup();
};

#endif