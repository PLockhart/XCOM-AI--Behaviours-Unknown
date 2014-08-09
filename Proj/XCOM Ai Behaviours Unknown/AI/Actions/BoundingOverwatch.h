#ifndef BoundingOverwatchH
#define BoundingOverwatchH

#include "../Action.h"

#include  "Patrol.h"
#include "SuppressAction.h"

class CoveringFire;

class BoundingOverwatch : public Patrol {

	//Variables
	//===================================================
public:
	bool IsPatrollerInPosition;	//Flag for whether the patrol action is ready to get on for the next leg of the journey
	AICharacter * CoveringCharacter;	//The character that the acting character is patrolling with

	//Methods
	//===================================================
public:
	BoundingOverwatch::BoundingOverwatch(AICharacter * char1, AICharacter * char2, int priority);
	void act(float dt, AICharacter * sender);
	void cancel();
	void deleted();
	void hardCancel();
	bool isSameKind(Action * other);
	bool shouldGiveWayTo(Action *other);

	std::string toString();

protected:
	void finished();

	vector<Tile*> getBestLOSCandidatesFor(AICharacter * character);

	void moverFinished();
};

#endif