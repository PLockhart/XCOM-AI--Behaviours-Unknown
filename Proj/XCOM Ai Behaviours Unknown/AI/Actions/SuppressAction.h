#ifndef SuppressActionH
#define SuppressActionH

#include "AttackAction.h"

class SuppressAction : public AttackAction {

	//Methods
	//===================================================
public:
	SuppressAction::SuppressAction(AICharacter * actor, AICharacter * target, int priority);
	bool isSameKind(Action * other);

	bool shouldGiveWayTo(Action *other);

	std::string toString();

private:
	void registerAttack();
	void performAttack();
};

#endif