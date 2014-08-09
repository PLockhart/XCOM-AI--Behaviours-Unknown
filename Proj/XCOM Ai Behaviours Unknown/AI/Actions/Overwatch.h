#ifndef OverwatchH
#define OverwatchH

#include "../Action.h"
#include "Attackaction.h"

class Overwatch : public Action {

	//Variables
	//===================================================
protected:
	AttackAction * _coveringAction;	//The attack action we will be using 
	bool _hasAction;	//Flag for whether the attack action has been instantiated

	//Methods
	//===================================================
public:
	Overwatch::Overwatch(AICharacter * actor, int priority);
	void act(float dt, AICharacter * sender);
	
	bool canInterrupt();
	bool canDoBoth(Action * other);
	void cancel();

	bool isSameKind(Action * other);
	bool shouldGiveWayTo(Action *other);

	virtual std::string toString();

protected:
	void setup();

	virtual AICharacter* chooseTarget();
	virtual void engageTarget(AICharacter * target);
};

#endif