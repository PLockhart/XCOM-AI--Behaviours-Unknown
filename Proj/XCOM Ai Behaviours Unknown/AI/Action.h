#ifndef ActionH
#define ActionH

#include <string>

#include "../Misc/aimath.h"
#include "../Level/Level.h"
class AICharacter;

enum ActionStates
{ kRunning, kCancelling, kFinished, kCancelled };

class Action {

	//Variables
	//===================================================
public:
	AICharacter * ActingCharacter;	//The character that is performing this action

	ActionStates State;	//The state of the action

	float Priority;	//The priorty of the action, from 0 to 100

protected:
	bool _isSetupFin;	//Flag for when the setup of this action has been performed

	//Methods
	//===================================================
public:
	Action::Action(int priority);
	virtual void Act(float dt, AICharacter * sender);
	virtual bool CanInterrupt() = 0;
	virtual bool CanDoBoth(Action * other) = 0;
	virtual void Cancel();
	virtual void HardCancel();	//Immediately cancels, only to be used in the event of character death
	virtual void Deleted();	//called before an action is deleted
	bool IsActionComplete();
	virtual Action * FrontAction();

	virtual bool IsSameKind(Action * other) = 0;
	virtual bool ShouldGiveWayTo(Action *other) = 0;

	virtual std::string ToString() = 0;

protected:
	virtual void Setup() = 0;
	virtual void Finished();	//Is triggered when the action has been completed
};

#endif