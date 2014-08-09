#include "WaitAction.h"

WaitAction::WaitAction(AICharacter * actor, int priority)
: Action(priority) {

}

void WaitAction::setup() {

}

void WaitAction::act(float dt, AICharacter * sender) {

	//a wait action does nothing
}

bool WaitAction::canInterrupt() {

	return true;
}

void WaitAction::cancel() {
	
	Action::cancel();
	Action::finished();
}

bool WaitAction::canDoBoth(Action * other) {

	return false;
}

bool WaitAction::isSameKind(Action * other) {

	if (WaitAction * derived = dynamic_cast<WaitAction*>(other))
		return true;

	return false;
}

bool WaitAction::shouldGiveWayTo(Action * other) {

	if (isSameKind(other) == true) {

		return false;
	}

	//if not the same type, prioritise the action with the bigger priority
	else if (other->Priority >= Priority)
		return true;

	return false;
}

std::string WaitAction::toString() {

	return "Waiting";
}