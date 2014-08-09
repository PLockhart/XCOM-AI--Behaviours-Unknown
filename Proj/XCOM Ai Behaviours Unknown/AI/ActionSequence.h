#ifndef ActionSequenceH
#define ActionSequenceH

#include <vector>
using namespace std;

#include "Action.h"


class ActionSequence : public Action {

	//Variables
	//===================================================
public:
	vector<Action*> ActionQueue;	//The queue of actions to be performed

	//Methods
	//===================================================
	ActionSequence::ActionSequence(int priority);
	void addAction(Action * newAct);

	void act(float dt, AICharacter * sender);
	bool canInterrupt();
	bool canDoBoth(Action * other);
	void cancel();
	void hardCancel();

	Action * frontAction();

	bool isSameKind(Action * other);
	bool shouldGiveWayTo(Action * other);

	std::string toString();

protected:
	void setup();
	virtual void finished();	//Is triggered when the action has been completed
};

#endif