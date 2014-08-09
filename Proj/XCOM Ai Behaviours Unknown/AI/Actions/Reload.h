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