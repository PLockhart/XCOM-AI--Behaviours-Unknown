#include "Level.h"

#include <iostream>
#include <istream>
#include <fstream>
#include <string>

#include "../AI/Raycast.h"

//Loads a level according to the provided schematic
Level::Level(char * fileName) {

	Description = "";
	ParseLevel(fileName);
}

Level::~Level() {

	for (int i = 0; i < TilesTall; i++) {
		for (int j = 0; j < TilesWide; j++)
			//delete the looped tile being pointed to
			delete TileGrid[i][j];

		//delete the row
		delete [] TileGrid[i];
	}

	//delete the array of rows
	delete [] TileGrid;

	for (int i = 0; i < TilesTall; i++)
		delete [] _losData[i];
	delete [] _losData;
}

//Parses a level blue print and creates a tile grid from it
void Level::ParseLevel(char * fileName) {

	//load in the file
	std::ifstream levelData;
	levelData.open(fileName);

	std::string levelInfo;

	//loop through all the lines, counting the height of the map
	TilesTall = 0;
	while (levelData.eof() == false) {

		std::getline(levelData, levelInfo);
		TilesTall++;
	}

	//get the last line of the map data and count how many characters there are across for the tiles wide
	TilesWide = levelInfo.size();

	//reset the file line pointer
	levelData.clear();
	levelData.seekg(0, std::istream::ios_base::beg);
	
	//using this info we can now create our tile grid
	TileGrid = new Tile**[TilesTall];
	for (int i = 0; i < TilesTall; i++)
		TileGrid[i] = new Tile*[TilesWide];

	//now read in the rest of the level data
	for (int row = 0; row < TilesTall; row++) {

		std::string loopedString;
		std::getline(levelData, loopedString);

		//loop through this lines characters getting the type from the char
		for (int i = 0; i < TilesWide; i++) {

			char indexChar = loopedString[i];
			CoverTypes type;

			bool isSpawnPoint = false;

			//parse the cover type
			switch (indexChar) {

			case 'L':
				type = kLow;

				break;
			case 'H':
				type = kHigh;

				break;
			case 'P':
				type = kOpen;
				isSpawnPoint = true;

				break;
			default:
				type = kOpen;

				break;
			}

			//create the tile
			TileGrid[row][i] = new Tile(type, i, row, this);

			if (isSpawnPoint == true)
				_spawnPositions.push_back(TileGrid[row][i]);
		}
	}

	//loop through all the tiles in the level and work out what neighbouring tiles provide cover
	for (int i = 0; i < TilesTall; i++) {
		for (int j = 0; j < TilesWide; j++) {

			if (i - 1 >= 0) {
				//north side
				if (TileGrid[i - 1][j]->CoverType != kOpen)
					TileGrid[i][j]->CoverNorth = true;
			}

			if (i + 1 < TilesTall) {
				//south side
				if (TileGrid[i + 1][j]->CoverType != kOpen)
					TileGrid[i][j]->CoverSouth = true;
			}

			if (j + 1 < TilesWide) {
				//east
				if (TileGrid[i][j + 1]->CoverType != kOpen)
					TileGrid[i][j]->CoverEast = true;
			}

			if (j - 1 >= 0) {
				//west
				if (TileGrid[i][j - 1]->CoverType != kOpen)
					TileGrid[i][j]->CoverWest = true;
			}
		}
	}
}

//Returns the tile that a specific spawn index has been assigned
Tile* Level::GetSpawnTile(int spawnIndex) {

	return _spawnPositions[spawnIndex];
}

//returns the number of spawn positions there are in the level
int Level::GetNumSpawnPos() {

	return (int)_spawnPositions.size();
}

//Returns a vector of LOS data for a specific tile
std::vector<Tile*> Level::GetLOSForTile(Tile * target) {

	return _losData[target->Y][target->X];
}

//Loads the graphics to represent a level with;
//The image to represent a open space
//the image to represent low cover
//the image to represent high cover
void Level::LoadContent(TGATexture blankSpace, TGATexture lowCover, TGATexture highCover) {

	//loop through all of the tiles and load the appropiate texture for it
	for (int i = 0; i < TilesTall; i++) {
		for (int j = 0; j < TilesWide; j++) {

			Tile * loopedTile = TileGrid[i][j];

			switch (loopedTile->CoverType) {

				case kLow:
					loopedTile->LoadContent(new Sprite2D(lowCover));
					break;

				case kHigh:
					loopedTile->LoadContent(new Sprite2D(highCover));
					break;

				default:
					loopedTile->LoadContent(new Sprite2D(blankSpace));
					break;
			}
		}
	}
	
	_losData = new std::vector<Tile*>*[TilesTall];

	for (int i = 0; i < TilesTall; i++)
		_losData[i] = new std::vector<Tile*>[TilesWide];

	//populate the line of sight data
	for (int i = 0; i < TilesTall; i++) {
		for (int j = 0; j < TilesWide; j++) {

			Tile * loopedTile = TileGrid[i][j];
			Raycast::VisibleTiles(_losData[i][j], loopedTile);
		}
	}
}

//Draws the level's tiles
void Level::Draw() {

	for (int i = 0; i < TilesTall; i++) {
		for (int j = 0; j < TilesWide; j++) {

			TileGrid[i][j]->Draw();
		}
	}

	//draw description
	glRasterPos2f(30, 20);

	for (int i = 0; i < (int)Description.size(); i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, Description[i]);

	//draw footnote
	glColor3f(1, 0, 0);

	Vector3 footnotePos = Vector3(40, 700, 0);
	glRasterPos2f(footnotePos.x, footnotePos.y);

	int maxCharsWide = 99;
	int currentCharsWide = 0;

	for (int i = 0; i < (int)Footnote.size(); i++, currentCharsWide++) {

		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, Footnote[i]);

		if (currentCharsWide > maxCharsWide && Footnote[i] != ' ') {

			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '-');

			currentCharsWide = -1;
			footnotePos.y += 15;
			glRasterPos2f(footnotePos.x, footnotePos.y);
		}
	}

	glColor3f(1, 1, 1);
}