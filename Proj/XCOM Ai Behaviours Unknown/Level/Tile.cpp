#include "Tile.h"

#include <math.h>

#include "../Level/Level.h"
#include "../AI/AICharacter.h"

#include <gl\glut.h>
#include <gl\GL.h>

//Creates a level tile with;
//its tile type
//its x and y positions in the 2d array contained in parent
//the level this tile belongs to
Tile::Tile(CoverTypes type, int x, int y, Level * parent) {

	CoverArc = 140;
	CoverType = type;
	X = x;
	Y = y;
	ParentLevel = parent;
	HasPathfindingParent = false;
	CoverNorth = CoverSouth = CoverEast = CoverWest = false;
	IsOccupied = false;

	FCost = GCost = HCost = Weighting = Exposure = 0;
}

Tile::~Tile() {

	delete TileSprite;
}

//Gets the tiles that this line can see
std::vector<Tile*> Tile::getLosTiles() {

	return ParentLevel->getLOSForTile(this);
}

//Checks to see if at a give angle of entry, the tile has that side protected by cover
bool Tile::isInCoverAtAngle(float angle) {

	if (CoverNorth == false &&
		CoverEast == false &&
		CoverSouth == false &&
		CoverWest == false)
		return false;

	Rotations::clampDegreeRotation(angle);

	if ((angle > 360 - CoverArc / 2 || angle < CoverArc / 2) && CoverNorth == true)
		return true;

	if ((angle > 90 - CoverArc / 2 && angle < 90 + CoverArc / 2) && CoverEast == true)
		return true;

	if ((angle > 180 - CoverArc / 2 && angle < 180 + CoverArc / 2) && CoverSouth == true)
		return true;

	if ((angle > 270 - CoverArc / 2 && angle < 270 + CoverArc / 2) && CoverWest == true)
		return true;

	return false;
}

bool Tile::isInCoverFrom(Vector3 attackOrigin) {

	//work out the angle from this tile to the attack
	Vector3 dir = attackOrigin - Position;

	//resolve vector into an angle, where 0 is facing up n degrees
	float attackAngle = ((atan2(dir.y, dir.x) * 180) / 3.1415926) + 90;

	return isInCoverAtAngle(attackAngle);
}

//Loads the sprite to represent this tile and also sets the position of the tile according to the sprites dimensions
void Tile::loadContent(Sprite2D * sprite) {
	
	//work out the centered position of this tile relative to the other tiles
	Position = Vector3((sprite->Width * X) + sprite->Width / 2, (sprite->Height * Y) + sprite->Height / 2, 0);
	Position = Position + Vector3(sprite->Width, sprite->Height, 0);

	TileSprite = sprite;
	//set the tile sprite position to be that from the top left, not centered
	TileSprite->Position = Position;
}

void Tile::draw() {

	TileSprite->draw();
}

//Pathfinding methods
/////////////////////////////////////////////////////////////////////////////////////

//Assigns and calculates the costs for moving to this tile
void Tile::assignCosts(int g, int h) {

	GCost = abs(g);
	HCost = abs(h);
	FCost = GCost + HCost;
	//FCost = GCost + HCost + Weighting;
}

void Tile::setPathfindingParent(Tile * parent) {

	PathfindingParent = parent;
	HasPathfindingParent = true;
}

void Tile::resetTile() {

	assignCosts(0, 0);
	Weighting = 0;
	Exposure = 0;
	HasPathfindingParent = false;
}

//Returns true if this tile has a lower movement cost than the compared tile
bool Tile::IsLower(Tile * i, Tile * j) {

	return (i->FCost < j->FCost);
}
