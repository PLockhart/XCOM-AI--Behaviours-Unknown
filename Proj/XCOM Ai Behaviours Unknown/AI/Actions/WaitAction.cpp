#include "WaitAction.h"

WaitAction::WaitAction(AICharacter * actor, int priority)
: Action(priority) {

}

void WaitAction::Setup() {

}

void WaitAction::Act(float dt, AICharacter * sender) {

	//a wait action does nothing
}

bool WaitAction::CanInterrupt() {

	return true;
}

void WaitAction::Cancel() {
	
	Action::Cancel();
	Action::Finished();
}

bool WaitAction::CanDoBoth(Action * other) {

	return false;
}

bool WaitAction::IsSameKind(Action * other) {

	if (WaitAction * derived = dynamic_cast<WaitAction*>(other))
		return true;

	return false;
}

bool WaitAction::ShouldGiveWayTo(Action * other) {

	if (IsSameKind(other) == true) {

		return false;
	}

	//if not the same type, prioritise the action with the bigger priority
	else if (other->Priority >= Priority)
		return true;

	return false;
}

std::string WaitAction::ToString() {

	return "Waiting";
}