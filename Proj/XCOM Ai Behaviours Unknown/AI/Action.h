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
	virtual void act(float dt, AICharacter * sender);
	virtual bool canInterrupt() = 0;
	virtual bool canDoBoth(Action * other) = 0;
	virtual void cancel();
	virtual void hardCancel();	//Immediately cancels, only to be used in the event of character death
	virtual void deleted();	//called before an action is deleted
	bool isActionComplete();
	virtual Action * frontAction();

	virtual bool isSameKind(Action * other) = 0;
	virtual bool shouldGiveWayTo(Action *other) = 0;

	virtual std::string toString() = 0;

protected:
	virtual void setup() = 0;
	virtual void finished();	//Is triggered when the action has been completed
};

#endif