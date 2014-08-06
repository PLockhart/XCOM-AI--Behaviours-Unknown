#ifndef DisregardsSafetyH
#define DisregardsSafetyH

#include "../Decision.h"
#include "../Action.h"
class DecisionTree;

class DisregardsSafety : public Decision {

	float _threatConstant;	//the constant is the unacceptable enemy threat when comparing our threat - their threat

	//Methods
	//===================================================
public:
	DisregardsSafety::DisregardsSafety(DecisionTree * tree, float threatConstant);
	Action* Run();
};

#endif