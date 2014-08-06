#include "DecisionTree.h"

#include "Decision.h"
#include "AICharacter.h"
#include "Decisions.h"
#include "Team.h"

#include <gl\glut.h>
#include <gl\GL.h>

//Sets up a decision tree
DecisionTree::DecisionTree(AICharacter * actor, Level * level) {

	Character = actor;
	CharTeam = actor->ParentTeam;
	GameLevel = level;

	//build the decision tree
	BuildTree();

	DebugEnabled = false;
}

//Builds the decision tree for a typical character
void DecisionTree::BuildTree() {

	//build tree across the branches from the bottom up

	SuppressEnemies * suppressEngage = new SuppressEnemies(this);
	suppressEngage->FalseBranch = new EngageEnemies(this);

	ShouldMoveInForKill * assaultEngage = new ShouldMoveInForKill(this, 750, 5);
	assaultEngage->TrueBranch = new MoveToEngage(this);
	assaultEngage->FalseBranch = suppressEngage;
	
	//===================================================

	//FindNearCombatCover * findImmediateCover = new FindNearCombatCover(this, 1);
	//findImmediateCover->FalseBranch = assaultEngage;

	//IsInCover * immediateCoverCheck = new IsInCover(this);
	//immediateCoverCheck->TrueBranch = assaultEngage;
	//immediateCoverCheck->FalseBranch = findImmediateCover;

	//===================================================

	DisregardsSafety * disregardSafety = new DisregardsSafety(this, 100);
	disregardSafety->TrueBranch = suppressEngage;
	disregardSafety->FalseBranch = new FallBack(this, 15);

	FindNearCombatCover * nearbyCoverCheck = new FindNearCombatCover(this, 3);
	nearbyCoverCheck->FalseBranch = disregardSafety;

	//===================================================

	Decision * inGoodCombatPos = new IsUnderLowThreat(this, -50);
	//inGoodCombatPos->TrueBranch = immediateCoverCheck;
	inGoodCombatPos->TrueBranch = assaultEngage;
	inGoodCombatPos->FalseBranch = nearbyCoverCheck;

	//===================================================

	Decision * personallyEngageEnemy = new EnemiesInRange(this, 4);
	personallyEngageEnemy->TrueBranch = inGoodCombatPos;
	personallyEngageEnemy->FalseBranch = new MoveToEngage(this);

	//===================================================

	Decision * ammoCover = new IsUnderLowThreat(this, -100);
	ammoCover->TrueBranch = new ReloadWeapon(this);
	ammoCover->FalseBranch = new MoveToSafePlace(this);

	//===================================================

	Decision * combatAmmoCheck = new HasMinAmmo(this, 10);
	combatAmmoCheck->FalseBranch = ammoCover;
	combatAmmoCheck->TrueBranch = personallyEngageEnemy;

	//===================================================

	Decision * unseenAmmoCheck = new HasMinAmmo(this, 60);
	unseenAmmoCheck->FalseBranch = new ReloadWeapon(this);
	unseenAmmoCheck->TrueBranch = new BeginPatrolling(this, 2);
	
	Decision * isInTooMuchDanger = new FeelsInDanger(this, 85);
	isInTooMuchDanger->TrueBranch = new FallBack(this, 5);
	isInTooMuchDanger->FalseBranch = combatAmmoCheck;

	//===================================================
	//the root
	Root = new EnemyPresense(this);
	Root->FalseBranch = unseenAmmoCheck;
	Root->TrueBranch = isInTooMuchDanger;
}

DecisionTree::~DecisionTree() {

	delete Root;
}

//Runs the decision tree
Action* DecisionTree::Run() {

	_recentLogs.clear();

	//return the resulted action from processing the tree
	Action * result = Root->Run();

	return result;
}

void DecisionTree::Log(string message) {

	if (DebugEnabled == true) {

		_recentLogs.push_back(message);
		//std::cout<< message << std::endl;
	}
}

//draw the thought process when going through this tree
void DecisionTree::Draw() {

	glPushMatrix();

	Vector3 drawPos = Character->Position;

	if (Character->ParentTeam->Identifier % 2 == 0)
		drawPos.x += 20;
	else
		drawPos.x -= 200;

	for (int i = 0; i < (int)_recentLogs.size(); i++) {

		//draw all the logs near the acting character
		glRasterPos2f(drawPos.x, drawPos.y);

		DrawString(_recentLogs[i]);

		drawPos.y += 10;
	}

	glPopMatrix();
}

void DecisionTree::DrawString(string input) {

	for (int i = 0; i < (int)input.size(); i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, input[i]);
}
