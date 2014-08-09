#ifndef TILEH
#define TILEH

#include <vector>
#include "../Misc/aimath.h"
#include "../Misc/Sprite2D.h"

class AICharacter;
class Level;

enum CoverTypes
{ kOpen, kLow, kHigh };

//Mutliple tiles make up a level. A tile can have cover in it and
//has calculation costs for ai movement
class Tile {

	//Variables
	//===================================================
public:
	CoverTypes CoverType;	//The type of cover this tile is
	Vector3 Position;	//The position of this tile, centered
	int X, Y;	//The x and y positions of this tile in the level's tile grid
	bool CoverNorth, CoverSouth, CoverEast, CoverWest;	//Flags for the different sides of the tile which are cover
	int CoverArc;	//The angle that cover extends over
	bool IsOccupied;	//Flag for whether a character is resting on this square 

	Sprite2D * TileSprite;	//The imate to represent the tile. It has its own position

	//pathfinding costs
	int FCost;	//combination of g and h costs
	int GCost;	//The guess distance from the start of the path
	int HCost;	//the guess distance to the end destination of the path
	int Weighting;	//Pathfinding modifier for fine tunning of tiles
	float Exposure;	//The weighting of exposure as a charaacter is travelling from a weighted tile to a weighted tile
	Tile * PathfindingParent;	//The tile that this tile comes form along a path to a goal
	bool HasPathfindingParent;	//Flag for weither the tile has a parent tile along the path its on

	Level * ParentLevel;	//The parent level this tile belongs to

	//Methods
	//===================================================
public:
	Tile::Tile();
	Tile::Tile(CoverTypes type, int x, int y, Level * parent);
	Tile::~Tile();
	std::vector<Tile*> getLosTiles();

	bool isInCoverAtAngle(float angle);
	bool isInCoverFrom(Vector3 attackOrigin);

	void loadContent(Sprite2D * sprite);
	void draw();

	//path finding methods
	void assignCosts(int g, int h);
	void setPathfindingParent(Tile * parent);
	static bool IsLower(Tile * i, Tile * j);
	void resetTile();
};

#endif