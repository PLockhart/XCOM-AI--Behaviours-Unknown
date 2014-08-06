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
	void Act(float dt, AICharacter * sender);
	
	bool CanInterrupt();
	bool CanDoBoth(Action * other);
	void Cancel();

	bool IsSameKind(Action * other);
	bool ShouldGiveWayTo(Action *other);

	virtual std::string ToString();

protected:
	void Setup();

	virtual AICharacter* ChooseTarget();
	virtual void EngageTarget(AICharacter * target);
};

#endif