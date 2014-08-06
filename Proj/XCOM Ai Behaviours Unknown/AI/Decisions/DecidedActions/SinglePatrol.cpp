#include "SinglePatrol.h"
#include "../../Actions/Patrol.h"
#include "../../DecisionTree.h"
#include "../../AICharacter.h"

SinglePatrol::SinglePatrol(DecisionTree * tree)
	: DecisionAction(tree) {

}

//Character should patrol for enemies
Action* SinglePatrol::Run() {

	return new Patrol(Tree->Character, 0);
}