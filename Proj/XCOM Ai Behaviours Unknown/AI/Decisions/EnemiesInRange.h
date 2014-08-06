#ifndef EnemiesInRangeH
#define EnemiesInRangeH

#include "../Decision.h"
#include "../Action.h"
class DecisionTree;

class EnemiesInRange : public Decision {

	//Methods
	//===================================================
	float _aggressionModifier;	//Modifier of the aggression, when comparing accuracies to targets to see if they are acceptable range

	//Methods
	//===================================================
public:
	EnemiesInRange::EnemiesInRange(DecisionTree * tree, float aggressionModifier);
	Action* Run();
};

#endif