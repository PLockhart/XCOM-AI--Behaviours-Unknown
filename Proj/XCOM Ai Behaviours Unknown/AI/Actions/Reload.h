#ifndef ReloadH
#define ReloadH

#include "../Action.h"

class Reload : public Action {

	//Variables
	//===================================================
private:
	bool _hasStarted;	//flag for whether the reload action has been started

	//Methods
	//===================================================
public:
	Reload::Reload(AICharacter * actor, int priority);
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