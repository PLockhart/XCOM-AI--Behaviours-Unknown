#include "AICharacter.h"

//An action is created with a priority
Action::Action(int priority) {

	Priority = priority;
	State = kRunning;
	_isSetupFin = false;
}

//Performs the action, where the sender is the character that is calling the act function
void Action::act(float dt, AICharacter * sender) {

	//if the action's setup hasn't been done before, perform it.
	//the setup can do things such as working out the path to the target etc
	if (_isSetupFin == false) {
		setup();
		_isSetupFin = true;
	}
}

//method called when an action has been completed
void Action::finished() {

	if (State == kRunning)
		State = kFinished;

	else if (State == kCancelling)
		State = kCancelled;
}

//cancels the action
void Action::cancel() {

	State = kCancelling;
}

//A hard cancel is similair to a cancel except the action is stopped immedtiately. This should only be used in the event of a character's death etc
void Action::hardCancel() {

	State = kCancelled;
}

//Method called when the action is released from memory
void Action::deleted() {

	ActingCharacter->HasAction = false;
}

//Checks to see if the action has been completed
bool Action::isActionComplete() {

	if (State == kFinished || State == kCancelled)
		return true;

	return false;
}

//This is used for subclasses of action which may hide actions.
//An action sequence is an action that has a vector of actions, frontAction will return the first action in the queue.
Action* Action::frontAction() {

	return this;
}