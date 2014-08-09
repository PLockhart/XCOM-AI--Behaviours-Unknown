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
	virtual void act(float dt, AICharacter * sender);
	virtual void cancel();
	virtual void hardCancel();
	virtual bool canInterrupt();
	virtual bool canDoBoth(Action * other);
	bool isSameKind(Action * other);
	bool shouldGiveWayTo(Action *other);

	virtual std::string toString();

protected:
	virtual void finished();

	virtual vector<Tile*> getBestLOSCandidatesFor(AICharacter * character);
	Tile* pickBestLOSTileFor(AICharacter * character);

	virtual void MoverAct(float dt, AICharacter * sender);
	virtual void moverFinished();

	virtual void setup();
};

#endif