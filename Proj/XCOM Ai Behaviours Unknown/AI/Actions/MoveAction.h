#ifndef MoveActionH
#define MoveActionH

#include "../Action.h"
class Tile;
using namespace std;

class MoveAction : public Action {

	//Variables
	//===================================================
public:
	Vector3 Destination;	//The over all destination of the movement action

protected:
	vector<Tile*> _moveTiles;	//The series of positions the action must move though
	Tile * _originalDestination;	//the destination we intended to move to

	//Methods
	//===================================================
public:
	MoveAction::MoveAction(AICharacter * character, Tile * destination, int priority);
	void act(float dt, AICharacter * sender);
	void cancel();
	bool canInterrupt();
	bool canDoBoth(Action * other);
	virtual bool isSameKind(Action * other);
	virtual bool shouldGiveWayTo(Action *other);

	virtual std::string toString();

protected:
	MoveAction::MoveAction(int priority);
	void finished();	//Is triggered when the action has been completed

protected:
	virtual bool getPathwayData(vector<Tile*> &pathway, Tile * start, Tile * destination);
	virtual Tile* getIdealFallback();
	virtual void setup();
};

#endif