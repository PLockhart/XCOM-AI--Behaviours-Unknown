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
	void Act(float dt, AICharacter * sender);
	void Cancel();
	void Deleted();
	void HardCancel();
	bool IsSameKind(Action * other);
	bool ShouldGiveWayTo(Action *other);

	std::string ToString();

protected:
	void Finished();

	vector<Tile*> GetBestLOSCandidatesFor(AICharacter * character);

	void MoverFinished();
};

#endif