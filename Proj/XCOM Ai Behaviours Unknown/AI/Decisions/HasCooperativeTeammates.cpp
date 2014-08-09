#include "HasCooperativeTeammates.h"
#include "../DecisionTree.h"
#include "../AICharacter.h"
#include "../Team.h"

//Checks to see if the user has any teammates who would be willing to work together
HasCooperativeTeammates::HasCooperativeTeammates(DecisionTree * tree, float minComradery)
: Decision(tree) {

	_minComradery = minComradery;
}

Action* HasCooperativeTeammates::run() {

	vector<AICharacter*> teammates = Tree->CharTeam->getTeammatesOf(Tree->Character);

	//loop through all the allies and if we have at least 1 cooperative one, return true
	for (int i = 0; i < (int)teammates.size(); i++) {

		if (teammates[i]->Comradery >= _minComradery)
			return TrueBranch->run();
	}

	return FalseBranch->run();
}