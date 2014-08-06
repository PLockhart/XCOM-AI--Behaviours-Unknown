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
	void AddAction(Action * newAct);

	void Act(float dt, AICharacter * sender);
	bool CanInterrupt();
	bool CanDoBoth(Action * other);
	void Cancel();
	void HardCancel();

	Action * FrontAction();

	bool IsSameKind(Action * other);
	bool ShouldGiveWayTo(Action * other);

	std::string ToString();

protected:
	void Setup();
	virtual void Finished();	//Is triggered when the action has been completed
};

#endif