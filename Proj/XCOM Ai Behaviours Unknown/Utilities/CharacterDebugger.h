#ifndef CharacterDebuggerH
#define CharacterDebuggerH

#include <vector>
#include <string>

#include "../Characters and Weapons/Character.h"

using namespace std;

class CharacterDebugger {

	//Variables
	//===================================================
private:
	vector<Character*> _characters;

	//Methods
	//===================================================
public:
	void addCharacter(Character * newChar);
	void draw();

private:
	void drawString(string input);
	string combineStrAndNum(string input, int num);
};

#endif