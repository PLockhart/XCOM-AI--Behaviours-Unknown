#ifndef SuppressActionH
#define SuppressActionH

#include "AttackAction.h"

class SuppressAction : public AttackAction {

	//Methods
	//===================================================
public:
	SuppressAction::SuppressAction(AICharacter * actor, AICharacter * target, int priority);
	bool IsSameKind(Action * other);

	bool ShouldGiveWayTo(Action *other);

	std::string ToString();

private:
	void RegisterAttack();
	void PerformAttack();
};

#endif