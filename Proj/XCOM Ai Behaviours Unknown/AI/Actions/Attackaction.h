#ifndef AttackActionH
#define AttackActionH

#include "../Action.h"
class AICharacter;
class Level;

class AttackAction : public Action {

	//Variables
	//===================================================
public:
	AICharacter * Target;

	//Methods
	//===================================================
public:
	AttackAction::AttackAction(AICharacter * actor, AICharacter * target, int priority);
	void Act(float dt, AICharacter * sender);
	
	bool CanInterrupt();
	bool CanDoBoth(Action * other);
	void Cancel();
	void HardCancel();

	virtual bool IsSameKind(Action * other);
	virtual bool ShouldGiveWayTo(Action *other);

	virtual std::string ToString();

protected:
	void Finished();	//Is triggered when the action has been completed
	void Setup();

private:
	virtual void RegisterAttack();
	virtual bool WeaponReady();
	virtual void PerformAttack();
};

#endif