#ifndef CharacterH
#define CharacterH

#include "../Misc/Sprite2D.h"
#include "../Misc/aimath.h"
#include "../AI/AICharacter.h"

class BaseWeapon;
class Tile;

//a basic character in the game, with an ai component
class Character : public Sprite2D, public AICharacter {

	//Variables
	//===================================================
public:
	int Health;	//The health of the character
	int MaxHealth;	//The max health the character can have/starts with

	//Methods
	//===================================================
public:
	Character::Character(char * fileName, Tile * startTile, BaseWeapon * weapon, Team * team);
	Character::~Character();
	void update(float dt);
	void draw();
	string toString();

	//overrides of ai behaviours and required functions
	void Character::moveBy(Vector3 move);
	void rotateBy(float amount);
	void faceTowards(Vector3 point);
	void damageTaken(AICharacter * source);

protected:
	void updateChildTraits();
};
#endif