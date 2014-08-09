#ifndef TacticalMoveH
#define TacticalMoveH

#include "MoveAction.h"

class TacticalMove : public MoveAction {

	//Methods
	//===================================================
public:
	TacticalMove::TacticalMove(AICharacter * character, Tile * destination, int priority);
	bool isSameKind(Action * other);
	bool shouldGiveWayTo(Action *other);

	std::string toString();

protected:
	bool getPathwayData(vector<Tile*> &pathway, Tile * start, Tile * destination);
	Tile* getIdealFallback();
};

#endif