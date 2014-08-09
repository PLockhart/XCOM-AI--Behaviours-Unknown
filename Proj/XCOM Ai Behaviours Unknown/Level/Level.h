#ifndef LEVELH
#define LEVELH

#include <vector>
#include <string>

#include "Tile.h"
#include "../Utilities/TGALoader.h"

//A level has a grid of tiles to represent places where ai can move
class Level {

	//Variables
	//===================================================
public:
	Tile *** TileGrid;	//An array of rows, where each row is an array of pointers to tiles
	int TilesWide, TilesTall;	//the dimensions of the level 
	std::string Description;	//Description of the level
	std::string Footnote;	//added commentary on the level

private:
	std::vector<Tile*> _spawnPositions;	//The spawn positions of characters, taken as sets of 2
	std::vector<Tile*>** _losData;	//the vector of tiles that each of the tiles in the grid can see from its location

	//Methods
	//===================================================
public:
	Level::Level(char * fileName);
	Level::~Level();
	Tile* getSpawnTile(int spawnIndex);
	int getNumSpawnPos();
	std::vector<Tile*> getLOSForTile(Tile * target);
	void loadContent(TGATexture blankSpace, TGATexture lowCover, TGATexture highCover);
	void draw();

protected:
	void parseLevel(char * fileName);
};

#endif