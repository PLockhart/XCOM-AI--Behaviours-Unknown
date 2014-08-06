#ifndef PatrolH
#define PatrolH

#include "../Action.h"
#include "MoveAction.h"

class Patrol : public Action {

	//Variables
	//===================================================
public:
	MoveAction * PatrolMove;	//The move action that the character will perform when going from point to point

private:
	vector<Tile*> _recentTiles;	//the tiles we have recentky moved to for los positions. This is used so we try not to visist the same place twice
	int _maxRecent;	//the max amount of recent tiles we keep track of

	//Methods
	//===================================================
public:
	Patrol::Patrol(AICharacter * character, int priority);
	virtual void Act(float dt, AICharacter * sender);
	virtual void Cancel();
	virtual void HardCancel();
	virtual bool CanInterrupt();
	virtual bool CanDoBoth(Action * other);
	bool IsSameKind(Action * other);
	bool ShouldGiveWayTo(Action *other);

	virtual std::string ToString();

protected:
	virtual void Finished();

	virtual vector<Tile*> GetBestLOSCandidatesFor(AICharacter * character);
	Tile* PickBestLOSTileFor(AICharacter * character);

	virtual void MoverAct(float dt, AICharacter * sender);
	virtual void MoverFinished();

	virtual void Setup();
};

#endif