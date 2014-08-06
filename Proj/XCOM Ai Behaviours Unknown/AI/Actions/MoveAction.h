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
	void Act(float dt, AICharacter * sender);
	void Cancel();
	bool CanInterrupt();
	bool CanDoBoth(Action * other);
	virtual bool IsSameKind(Action * other);
	virtual bool ShouldGiveWayTo(Action *other);

	virtual std::string ToString();

protected:
	MoveAction::MoveAction(int priority);
	void Finished();	//Is triggered when the action has been completed

protected:
	virtual bool GetPathwayData(vector<Tile*> &pathway, Tile * start, Tile * destination);
	virtual Tile* GetIdealFallback();
	virtual void Setup();
};

#endif