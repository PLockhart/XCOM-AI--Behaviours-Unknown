#include "ActionSequence.h"

//an actionsequence is a series of action that is to be performed one after the other
ActionSequence::ActionSequence(int priority) 
	: Action(priority) {

}

void ActionSequence::setup() {

	_isSetupFin = true;
}

//Adds an action to the action queue
void ActionSequence::addAction(Action * newAct) {

	//if an actionsequence was added, just add all the sequence's actions
	if (ActionSequence * derived = dynamic_cast<ActionSequence*>(newAct)) {

		for (int i = 0; i < (int)derived->ActionQueue.size(); i++)
			ActionQueue.push_back(derived->ActionQueue[i]);
	}
	//else normally just add the action
	else {

		ActionQueue.push_back(newAct);
	}
}

//performs the actions in the sequence
void ActionSequence::act(float dt, AICharacter * sender) {

	if (ActionQueue.size() > 0) {

		//get the action at the front of the queue and perform it
		Action * currentAct = ActionQueue[0];
		currentAct->act(dt, sender);

		//if the action has been performed
		if (currentAct->isActionComplete() == true) {

			//if it was cancelled, cancel the other actions in the sequence
			if (currentAct->State == kCancelled)
				cancel();

			//remove it from the queue
			ActionQueue.erase(ActionQueue.begin());
			currentAct->deleted();
			delete currentAct;

			//if queue is empty, then the sequence has finished
			if ((int)ActionQueue.size() == 0)
				finished();
		}
	}
	else finished();
}

//Checks to see if the current action can be cancelled
bool ActionSequence::canInterrupt() {

	if (ActionQueue.size() > 0)
		return ActionQueue[0]->canInterrupt();
	
	else
		return true;
}

//checks to see if the provided action can be ran along side the sequence
bool ActionSequence::canDoBoth(Action * other) {

	for (int i = 0; i < (int)ActionQueue.size(); i++) {

		if (ActionQueue[i]->canDoBoth(other) == false)
			return false;
	}

	return true;
}

//event for when the sequence finishes
void ActionSequence::finished() {

	Action::finished();
}

//Cancels the currently executing action and removes the rest
void ActionSequence::cancel() {

	if (ActionQueue.size() > 0) {

		Action::cancel();
		ActionQueue[0]->cancel();

		//remove the other actions
		while (ActionQueue.size() > 1) {

			Action * loopedAction = ActionQueue[1];
			loopedAction->hardCancel();
			ActionQueue.erase(ActionQueue.begin() + 1);
			delete loopedAction;
		}
	}
}

//instantly cancels the current action and removes all others
void ActionSequence::hardCancel() {

	if (ActionQueue.size() > 0) {

		ActionQueue[0]->hardCancel();

		while (ActionQueue.size() > 0) {

			Action * loopedAction = ActionQueue[0];
			loopedAction->hardCancel();
			ActionQueue.erase(ActionQueue.begin());
			delete loopedAction;
		}
	}

	State = kCancelled;
}

bool ActionSequence::isSameKind(Action * other) {

	if (ActionSequence * derived = dynamic_cast<ActionSequence*>(other))
		return true;

	return false;
}

bool ActionSequence::shouldGiveWayTo(Action * other) {

	//if they are of the same type..
	if (isSameKind(other) == true) {

		ActionSequence * otherSeq = dynamic_cast<ActionSequence*>(other);

		//return true if this action sequence has finished
		if (ActionQueue.size() == 0)
			return true;

		//else base the give way off the first actions of the queues
		else
			return ActionQueue[0]->shouldGiveWayTo(otherSeq->ActionQueue[0]);
	}

	//if not the same type, prioritise the action with the bigger priority
	else if (other->Priority >= Priority)
		return true;

	return false;
}

//returns the first action in the queue, or this action sequence if no actions are in the queue
Action* ActionSequence::frontAction() {

	if (ActionQueue.size() > 0)
		return ActionQueue[0];
	else
		return this;
}

std::string ActionSequence::toString() {

	if (ActionQueue.size() > 1)
		return "Sq: " + ActionQueue[0]->toString();

	else if (ActionQueue.size() == 1)
		return ActionQueue[0]->toString();

	else
		return "";
}