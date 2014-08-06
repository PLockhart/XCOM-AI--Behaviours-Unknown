#include "Character.h"

#include <sstream>
#include <iostream>

#include "../AI/Team.h"
#include "../Utilities/Texture.h"
#include "../Level/Tile.h"
#include "BaseWeapon.h"
#include "../AI/DecisionTree.h"

//Creates a character with;
//the file path to the sprite to represent the character
//the tile the player should start at
//the weapon the player should have
Character::Character(char * fileName, Tile * startTile, BaseWeapon * weapon, Team * team)
	: Sprite2D(Texture::LoadTexture(fileName)) {

		//set up the positions
		Sprite2D::Position = startTile->Position;
		AICharacter::Position = startTile->Position;

		Weapon = weapon;
		Weapon->Holder = this;
		AICharacter::RegisterWeapon(weapon);

		//set up ai vars
		AICharacter::DestinationTile = startTile;
		AICharacter::CurrentTile = startTile;
		startTile->IsOccupied = true;
		AICharacter::MovedToTile(startTile);
		AICharacter::RegisterTeam(team);
		AICharacter::TheDecisionTree = new DecisionTree(this, startTile->ParentLevel);

		//default values
		Sprite2D::Rotation = 0;
		AICharacter::Rotation = 0;
		Health = MaxHealth = 20;
}

//Updates the character
void Character::Update(float dt) {

	//match the ai positions and rotations to that of this character
	AICharacter::Position = Sprite2D::Position;
	AICharacter::Rotation = Sprite2D::Rotation;

	//only perform the AI if we are not dead
	if (IsDead == false)
		AICharacter::Update(dt);
}

//Updates traits of the ai character based on this classes' variables
void Character::UpdateChildTraits() {

	//reduce the boldness of a character depending on their health
	float hpRatio = Health / (float)MaxHealth;

	if (hpRatio <= 0.4f) {

		Boldness = max(Boldness - 4, 0);
		Aggression = max(Aggression - 2, 0);
	}

	else if (hpRatio <= 0.6f) {

		Boldness = max(Boldness - 2, 0);
		Aggression = max(Aggression - 1, 0);
	}
}

//Method for when damage is taken from
//the character holding the weapon
void Character::DamageTaken(AICharacter * source) {

	if (Health > 0)
		Health--;

	if (Health <= 0 && IsDead == false) {

		//mark character as dead and make them display dead
		IsDead = true;
		Sprite2D::DisplayColour.a = 0.1f;

		//delete the current action if we have one
		if (HasAction == true) {

			CurrentAct->HardCancel();
			delete CurrentAct;
			HasAction = false;
		}

		if (CurrentTile == DestinationTile)
			CurrentTile->IsOccupied = false;

		//remove character from its team
		ParentTeam->RemoveMember(this);
	}
}

void Character::Draw() {

	//draw the character
	Sprite2D::Draw();

	//draw the weapon and the decision tree though process
	if (IsDead == false) {

		Weapon->Draw();
		TheDecisionTree->Draw();
	}
}

string Character::ToString() {
	
	std::stringstream output;
	output << "Team ";
	output << AICharacter::ParentTeam->Identifier;
	output << " ";
	output << Weapon->ToString();
	output << "man";

	return output.str();
}

//AI Behaviour overrides and required functions
//=============================================================

//Moves the character by a vector
void Character::MoveBy(Vector3 move) {

	if (move == Vector3(0, 0, 0))
		return;
	
	//face towards the point we are moving too
	FaceTowards(Sprite2D::Position + move);

	//move the sprite
	Sprite2D::Position = Sprite2D::Position + move;
}

//Rotates the character by an amount of 
void Character::RotateBy(float amount) {

	Sprite2D::Rotation += amount;
}

//rotates the character to face a point
void Character::FaceTowards(Vector3 point) {

	//work out the direction we have to face
	Vector3 difference = point - Sprite2D::Position;

	//resolve vector into an angle, where 0 is facing up n degrees
	Sprite2D::Rotation = ((atan2(difference.y, difference.x) * 180) / 3.1415926) + 90;
}

Character::~Character() {
}
