#include "ReloadWeapon.h"
#include "../../Actions/Reload.h"
#include "../../DecisionTree.h"
#include "../../AICharacter.h"

ReloadWeapon::ReloadWeapon(DecisionTree * tree)
	: DecisionAction(tree) {

}

//Character should reload their weapon
Action* ReloadWeapon::run() {

	Tree->log("Should reload");
	return new Reload(Tree->Character, 10);
}