#ifndef ShouldMoveInForKillH
#define ShouldMoveInForKillH

#include "../Decision.h"
#include "../Action.h"
class DecisionTree;

class ShouldMoveInForKill : public Decision {

	//Variables
	//===================================================
private:
	float _maxRange;	//Max distance away anenemy can be to consider assaulting it
	float _minAggression;	//min aggression needed before a character will consider assaulting

	//Methods
	//===================================================
public:
	ShouldMoveInForKill::ShouldMoveInForKill(DecisionTree * tree, float maxRange, float minAggression);
	Action* run();
};

#endif