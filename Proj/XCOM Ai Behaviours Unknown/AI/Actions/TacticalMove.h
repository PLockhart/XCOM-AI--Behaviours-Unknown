#ifndef TacticalMoveH
#define TacticalMoveH

#include "MoveAction.h"

class TacticalMove : public MoveAction {

	//Methods
	//===================================================
public:
	TacticalMove::TacticalMove(AICharacter * character, Tile * destination, int priority);
	bool IsSameKind(Action * other);
	bool ShouldGiveWayTo(Action *other);

	std::string ToString();

protected:
	bool GetPathwayData(vector<Tile*> &pathway, Tile * start, Tile * destination);
	Tile* GetIdealFallback();
};

#endif