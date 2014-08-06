#include "ReloadWeapon.h"
#include "../../Actions/Reload.h"
#include "../../DecisionTree.h"
#include "../../AICharacter.h"

ReloadWeapon::ReloadWeapon(DecisionTree * tree)
	: DecisionAction(tree) {

}

//Character should reload their weapon
Action* ReloadWeapon::Run() {

	Tree->Log("Should reload");
	return new Reload(Tree->Character, 10);
}