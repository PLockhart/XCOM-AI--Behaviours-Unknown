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
	void AddCharacter(Character * newChar);
	void Draw();

private:
	void DrawString(string input);
	string CombineStrAndNum(string input, int num);
};

#endif