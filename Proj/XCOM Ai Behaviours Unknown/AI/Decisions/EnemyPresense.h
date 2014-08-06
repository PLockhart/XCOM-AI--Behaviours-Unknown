#ifndef EnemyPresenseH
#define EnemyPresenseH

#include "../Decision.h"

class EnemyPresense : public Decision {

public:
	EnemyPresense::EnemyPresense(DecisionTree * tree);
	Action* Run();
};

#endif