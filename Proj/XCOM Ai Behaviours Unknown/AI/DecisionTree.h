#ifndef DecisionTreeH
#define DecisionTreeH

#include <iostream>
#include <string>
#include <vector>

class Decision;
class AICharacter;
class Team;
class Level;
class Action;

using namespace std;

class DecisionTree {

	//Variables
	//===================================================
public:
	Decision * Root;	//The start node of this decision tree
	AICharacter * Character;	//The character that this decision tree belongs to
	Team * CharTeam;	//The team the acting character belongs to
	Level * GameLevel;	//The level that the decisions are being made in

	bool DebugEnabled;

protected:
	vector<string> _recentLogs;	//recent logs after running through the decision tree

	//Methods
	//===================================================
public:
	DecisionTree::DecisionTree(AICharacter * actor, Level * level);
	DecisionTree::~DecisionTree();
	Action* Run();	//Runs through the decision tree, returning the action the actor should take
	void Log(string message);

	void Draw();

protected:
	void BuildTree();
	void DrawString(string input);
};

#endif