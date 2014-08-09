#include "../../Decision.h"

class ReloadWeapon : public DecisionAction {

public:
	ReloadWeapon::ReloadWeapon(DecisionTree * tree);
	Action* run();
};