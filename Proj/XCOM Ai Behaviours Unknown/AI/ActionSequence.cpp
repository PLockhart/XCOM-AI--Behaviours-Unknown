#include "ActionSequence.h"

//an actionsequence is a series of action that is to be performed one after the other
ActionSequence::ActionSequence(int priority) 
	: Action(priority) {

}

void ActionSequence::Setup() {

	_isSetupFin = true;
}

//Adds an action to the action queue
void ActionSequence::AddAction(Action * newAct) {

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
void ActionSequence::Act(float dt, AICharacter * sender) {

	if (ActionQueue.size() > 0) {

		//get the action at the front of the queue and perform it
		Action * currentAct = ActionQueue[0];
		currentAct->Act(dt, sender);

		//if the action has been performed
		if (currentAct->IsActionComplete() == true) {

			//if it was cancelled, cancel the other actions in the sequence
			if (currentAct->State == kCancelled)
				Cancel();

			//remove it from the queue
			ActionQueue.erase(ActionQueue.begin());
			currentAct->Deleted();
			delete currentAct;

			//if queue is empty, then the sequence has finished
			if ((int)ActionQueue.size() == 0)
				Finished();
		}
	}
	else Finished();
}

//Checks to see if the current action can be cancelled
bool ActionSequence::CanInterrupt() {

	if (ActionQueue.size() > 0)
		return ActionQueue[0]->CanInterrupt();
	
	else
		return true;
}

//checks to see if the provided action can be ran along side the sequence
bool ActionSequence::CanDoBoth(Action * other) {

	for (int i = 0; i < (int)ActionQueue.size(); i++) {

		if (ActionQueue[i]->CanDoBoth(other) == false)
			return false;
	}

	return true;
}

//event for when the sequence finishes
void ActionSequence::Finished() {

	Action::Finished();
}

//Cancels the currently executing action and removes the rest
void ActionSequence::Cancel() {

	if (ActionQueue.size() > 0) {

		Action::Cancel();
		ActionQueue[0]->Cancel();

		//remove the other actions
		while (ActionQueue.size() > 1) {

			Action * loopedAction = ActionQueue[1];
			loopedAction->HardCancel();
			ActionQueue.erase(ActionQueue.begin() + 1);
			delete loopedAction;
		}
	}
}

//instantly cancels the current action and removes all others
void ActionSequence::HardCancel() {

	if (ActionQueue.size() > 0) {

		ActionQueue[0]->HardCancel();

		while (ActionQueue.size() > 0) {

			Action * loopedAction = ActionQueue[0];
			loopedAction->HardCancel();
			ActionQueue.erase(ActionQueue.begin());
			delete loopedAction;
		}
	}

	State = kCancelled;
}

bool ActionSequence::IsSameKind(Action * other) {

	if (ActionSequence * derived = dynamic_cast<ActionSequence*>(other))
		return true;

	return false;
}

bool ActionSequence::ShouldGiveWayTo(Action * other) {

	//if they are of the same type..
	if (IsSameKind(other) == true) {

		ActionSequence * otherSeq = dynamic_cast<ActionSequence*>(other);

		//return true if this action sequence has finished
		if (ActionQueue.size() == 0)
			return true;

		//else base the give way off the first actions of the queues
		else
			return ActionQueue[0]->ShouldGiveWayTo(otherSeq->ActionQueue[0]);
	}

	//if not the same type, prioritise the action with the bigger priority
	else if (other->Priority >= Priority)
		return true;

	return false;
}

//returns the first action in the queue, or this action sequence if no actions are in the queue
Action* ActionSequence::FrontAction() {

	if (ActionQueue.size() > 0)
		return ActionQueue[0];
	else
		return this;
}

std::string ActionSequence::ToString() {

	if (ActionQueue.size() > 1)
		return "Sq: " + ActionQueue[0]->ToString();

	else if (ActionQueue.size() == 1)
		return ActionQueue[0]->ToString();

	else
		return "";
}