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
	void act(float dt, AICharacter * sender);
	
	bool canInterrupt();
	bool canDoBoth(Action * other);
	void cancel();
	void hardCancel();

	virtual bool isSameKind(Action * other);
	virtual bool shouldGiveWayTo(Action *other);

	virtual std::string toString();

protected:
	void finished();	//Is triggered when the action has been completed
	void setup();

private:
	virtual void registerAttack();
	virtual bool isWeaponReady();
	virtual void performAttack();
};

#endif