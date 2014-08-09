#include "Raycast.h"

#include <math.h>
#include <algorithm>

#include "../Level/Level.h"
#include "../Level/Tile.h"

//Casts a ray, returns true if could reach the destination. Works with;
//the tile we are casting from
//the tile we are casting to
bool Raycast::castRay(Tile * source, Tile *destination) {

	Level * level = source->ParentLevel;

	int x0 = source->X;
	int x1 = destination->X;
	int y0 = source->Y;
	int y1 = destination->Y;
	
	bool steep = abs(y1 - y0) > abs(x1 - x0);

    if (steep) {
        swap(x0, y0);
        swap(x1, y1);
    }
    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    int deltax = x1 - x0;
    int deltay = abs(y1 - y0);
    int error = 0;
    int ystep;
    int y = y0;

    if (y0 < y1) 
		ystep = 1; 
	else 
		ystep = -1;

	//loop along the line
    for (int x = x0; x <= x1; x++) {

		Tile * loopedTile;

		//get the tile along the line, reversing the coords if the calculated line was steep
        if (steep)
			loopedTile = level->TileGrid[x][y];
        else 
			loopedTile = level->TileGrid[y][x];

		//ray casts can't go through walls
		if (loopedTile->CoverType == kHigh)
			return false;

		//check to see if we have moved enough along the x to move up/down the y axis
        error += deltay;
        if (2 * error >= deltax) {
            y += ystep;
            error -= deltax;
        }
	}

	return true;
}

//Populates a list of all the tiles that are visible from a source tile in a level. The most accuracte out of the ones below
void Raycast::visibleTiles(vector<Tile*> &visibleTiles, Tile * source) {

	Level * level = source->ParentLevel;

	//add the source tile
	visibleTiles.push_back(source);

	//loop through all the tiles in the level
	for (int i = 0; i < level->TilesTall; i++) {
		for (int j = 0; j < level->TilesWide; j++) {

			Tile * loopedTile = level->TileGrid[i][j];

			//if not the tile we are coming from..
			if (loopedTile != source) {

				//if we can see it, add it to visible list
				if (Raycast::castRay(source, loopedTile))
					visibleTiles.push_back(loopedTile);
			}
		}
	}
}

/*
OTHER WORKING IMPLEMENTATIONS with varying degrees of accuracy

Using raycasts to the edge of the level from the source. Didn't work well
//Populates a list of all the tiles that are visible from a source tile in a level
void Raycast::visibleTiles(vector<Tile*> &visibleTiles, Tile * source) {

	Level * level = source->ParentLevel;

	//cast rays out around the source to the edges of the map
	for (int i = 0; i < level->TilesTall; i++) {
		
		int rowPosStep = level->TilesWide;
		//if the row is either the top or the bott, then we need to loop across all the tiles
		//instead of just the left adn right most tiles
		if (i == 0 || i == level->TilesTall - 1)
			rowPosStep = 1;

		for (int j = 0; j < level->TilesWide; j+= rowPosStep) {

			Tile * destination = level->TileGrid[i][j];
			vector<Tile*> tilesInLine;

			int x0 = source->X;
			int x1 = destination->X;
			int y0 = source->Y;
			int y1 = destination->Y;
	
			bool steep = abs(y1 - y0) > abs(x1 - x0);

			if (steep) {
				swap(x0, y0);
				swap(x1, y1);
			}
			if (x0 > x1) {
				swap(x0, x1);
				swap(y0, y1);
			}

			int deltax = x1 - x0;
			int deltay = abs(y1 - y0);
			int error = 0;
			int ystep;
			int y = y0;

			if (y0 < y1) 
				ystep = 1; 
			else 
				ystep = -1;

			for (int x = x0; x <= x1; x++) {

				Tile * loopedTile;

				if (steep)
					loopedTile = level->TileGrid[x][y];
				else 
					loopedTile = level->TileGrid[y][x];

				tilesInLine.push_back(loopedTile);

				error += deltay;
				if (2 * error >= deltax) {
					y += ystep;
					error -= deltax;
				}
			}

			//if the destination is at the start of the list, reverse it
			if (tilesInLine[0] == destination)
				std::reverse(tilesInLine.begin(), tilesInLine.end());

			//now loop along the tiles and adding them to the visible list until we hit the a tile that isn't visible
			for (int rayTile = 0; rayTile < (int)tilesInLine.size(); rayTile++) {

				Tile * loopedTile = tilesInLine[rayTile];

				//if not high then its a candidate
				if (loopedTile->CoverType != kHigh) {

					//if its not already in the visible tile list and hasn't already been added to the hidden tiles list...
					if (std::find(visibleTiles.begin(), visibleTiles.end(), loopedTile) == visibleTiles.end())
						visibleTiles.push_back(loopedTile);
				}
				else
					break;
			}
		}
	}

	//return true;
}
*/

/*Using angle and direction vectors to traverse a level. Worked ok
//Populates a list of all the tiles that are visible from a source tile in a level
void Raycast::visibleTiles(vector<Tile*> &visibleTiles, Tile * source) {

	Level * level = source->ParentLevel;
	vector<Tile*> hiddenTiles;

	//cast rays out around the source to the edges of the map
	for (int i = 0; i < level->TilesTall; i++) {
		
		int rowPosStep = level->TilesWide;
		//if the row is either the top or the bott, then we need to loop across all the tiles
		//instead of just the left adn right most tiles
		if (i == 0 || i == level->TilesTall - 1)
			rowPosStep = 1;

		for (int j = 0; j < level->TilesWide; j+= rowPosStep) {

			Vector3 dir = Vector3((j - source->X), i - source->Y, 0);
			dir.normalise();

			float x = dir.x;
			float y = dir.y;

			//the x and y indexes of a tile along the ray line, starting from the source
			float lineX = source->X;
			float lineY = source->Y;

			//loop along the direction until we hit the edge of the map
			bool hitImpassable = false;

			while (hitImpassable == false) {

				//check if the index is still in bounds
				if (lineX >= 0 && lineX < level->TilesWide && lineY >= 0 && lineY < level->TilesTall) {

					Tile * loopedTile = level->TileGrid[(int)lineY][(int)lineX];

					//we can't see past high cover, so end now if thats the case
					if (loopedTile->CoverType != kHigh) {

						//if its not already in the visible tile list and hasn't already been added to the hidden tiles list...
						if (std::find(visibleTiles.begin(), visibleTiles.end(), loopedTile) == visibleTiles.end() &&
							std::find(hiddenTiles.begin(), hiddenTiles.end(), loopedTile) == hiddenTiles.end())
							visibleTiles.push_back(loopedTile);
					}
					else {

						//we need to loop through the rest of this direction vector and mark all the tiles as hidden
						while (lineX >= 0 && lineX < level->TilesWide && lineY >= 0 && lineY < level->TilesTall) {

							loopedTile = level->TileGrid[(int)lineY][(int)lineX];

							//if its not already in the hidden tile list
							if (std::find(hiddenTiles.begin(), hiddenTiles.end(), loopedTile) == hiddenTiles.end())
								hiddenTiles.push_back(loopedTile);

							//if the tile has previously been marked as visible, it should be removed
							if (std::find(visibleTiles.begin(), visibleTiles.end(), loopedTile) != visibleTiles.end())
								visibleTiles.erase(std::remove(visibleTiles.begin(), visibleTiles.end(), loopedTile));

							//move along the direction
							lineX += x;
							lineY += y;
						}
					}
				}

				else
					hitImpassable = true;

				if (x== 0 && y == 0)
					break;

				//move along the direction
				lineX += x/1;
				lineY += y/1;
			}
		}
	}
}
*/
