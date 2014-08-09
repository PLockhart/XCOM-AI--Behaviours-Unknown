#include "PathFinding.h"

#include <math.h>
#include <algorithm>

#include "../Level/Level.h"
#include "../Level/Tile.h"

//Floods the map around the source tile, returning what tiles are within range of a certain amount of moves
//the max moves variable is assumed the number of standard moves, where a standard length of a connection is 10
void PathFinding::floodMap(vector<Tile*> &avaliableTiles, Tile * start, int maxMoves) {

	//work out the max costs for the flooding
	int maxCost = maxMoves * 10;

	Level * level = start->ParentLevel;

	//set up the vectors
	vector<Tile*> openTiles, closedTiles;
	avaliableTiles.clear();

	//add the initial tile
	openTiles.push_back(start);

	//keep going while we have tiles open
	while ((int)openTiles.size() != 0) {

		//sort all the tiles by cost
		std::sort(openTiles.begin(), openTiles.end(), Tile::IsLower);

		Tile * currentSquare = openTiles[0];

		//add it to the closed list
		closedTiles.push_back(currentSquare);

		//remove the current square from the open list since we have trabelled there
		openTiles.erase(std::find(openTiles.begin(), openTiles.end(), currentSquare));

		//loop through all the surrounding tiles of the current square
		for (int x = currentSquare->X - 1; x < currentSquare->X + 2; x++) {
			for (int y = currentSquare->Y - 1; y < currentSquare->Y + 2; y++) {

				//check that the indexes are within the bounds of the level arrays
				if ((x >= 0) && (x < level->TilesWide) && (y >= 0) && (y < level->TilesTall)) {

					//only proceed with this tile if it is walkable on and if we haven't visited it before (ie it won't be in the closed list)
					if (level->TileGrid[y][x]->CoverType == kOpen && std::find(closedTiles.begin(), closedTiles.end(), level->TileGrid[y][x]) == closedTiles.end()) {

						//we have a valid tile we can move to, so work out the deeper pathway code to carry on the path finding
						//I am setting the final destination parameter to be the same tile we are going to. That way, the estimated cost (which is ignored for dijkstra....
						///..'s algorithm does not use
						if (PathFinding::workoutPathwayDataToTargetTile(level, level->TileGrid[y][x], currentSquare, openTiles, x, y)) {

							//the looped tile was a sucess. Check to see if it meets cost requirements, and if not then remove it
							Tile * sucessTile = level->TileGrid[y][x];

							if (sucessTile->FCost > maxCost) {

								openTiles.erase(openTiles.end() - 1);
								sucessTile->assignCosts(0, 0);
								sucessTile->HasPathfindingParent = false;
							}
							else
								avaliableTiles.push_back(sucessTile);
						}

					}
				}
			}
		}
	}

	resetUsedTiles(closedTiles, openTiles);
}

//RETURNS false if no path can be found
//Method works out pathway from start position to end position with;
//The vector that will be populated with the route tiles
//the tile the path should start from
//the tile the path sohuld end at
bool PathFinding::getPathway(vector<Tile*> &pathway, Tile * start, Tile * destination) {

	pathway.clear();
	Level * level = start->ParentLevel;
	vector<Tile*> closedTiles;
	vector<Tile*> openTiles;

	if (start == destination) {

		pathway.push_back(destination);
		return true;
	}

	if (destination->CoverType != kOpen)
		return false;

	//work out the initial costs for this source tile
	int xCost = abs(destination->X - start->X);
	int yCost = abs(destination->Y - start->Y);
	start->assignCosts(start->GCost, (xCost + yCost) * 10);

	//add it to list of pathway tiles
	openTiles.push_back(start);

	//keep ging through all the tiles we can until we get to the destination
	while (std::find(closedTiles.begin(), closedTiles.end(), destination) == closedTiles.end()) {

		//ifwe have no more options available, return false. We have failed our single purpose in life
		if (openTiles.size() == 0)
			return false;

		//sort all the tiles by fcost
		std::sort(openTiles.begin(), openTiles.end(), Tile::IsLower);

		//set the current tile to be that of the lowest cost
		Tile * currentSquare = openTiles[0];
		//add it to the closed list
		closedTiles.push_back(currentSquare);

		//if we have found our destination
		if (currentSquare == destination) {

			PathFinding::foundDestination(currentSquare, pathway, closedTiles, openTiles);

			return true;
		}
		
		//still searching...

		//remove the current square from the open list since we have trabelled there
		openTiles.erase(std::find(openTiles.begin(), openTiles.end(), currentSquare));
		
		//loop through all the surrounding tiles of the current square
		for (int x = currentSquare->X - 1; x < currentSquare->X + 2; x++) {
			for (int y = currentSquare->Y - 1; y < currentSquare->Y + 2; y++) {

				//check that the indexes are within the bounds of the level arrays
				if ((x >= 0) && (x < level->TilesWide) && (y >= 0) && (y < level->TilesTall)) {

					//only proceed with this tile if it is walkable on and if we haven't visited it before (ie it won't be in the closed list)
					if (level->TileGrid[y][x]->CoverType == kOpen && std::find(closedTiles.begin(), closedTiles.end(), level->TileGrid[y][x]) == closedTiles.end())
						//we have a valid tile we can move to, so work out the deeper pathway code to carry on the path finding
						PathFinding::workoutPathwayDataToTargetTile(level, destination, currentSquare, openTiles, x, y);
				}
			}
		}
	}

	return false;
}

//RETURNS false if no path can be found
//Method works out pathway from start position to end position with, having infulenced the level with the vector of infulence data;
//The vector that will be populated with the route tiles
//the tile the path should start from
//the tile the path sohuld end at
bool PathFinding::getTacticalPathway(vector<InfulenceData> levelInfulence, vector<Tile*> &pathway, Tile * start, Tile * destination) {

	//loop through all of the infulence data and modify the tiles gcost by them
	for (int i = 0; i < (int)levelInfulence.size(); i++)
		levelInfulence[i].TheTile->Weighting = levelInfulence[i].Infulence;

	bool result = PathFinding::getPathway(pathway, start, destination);

	//reset all of the tiles we have infulenced as they may not have been in the open or closed lists
	PathFinding::resetInfulencedTiles(levelInfulence);

	return result;
}

//Given a current tile and a neighbouring tile on the grid this works out what valid tiles we can now move to and adds them to the open list with;
//the level to traverse though
//the final tile we aim to get to
//the tile we are currently on
//the lsit of tiles we can move through
//The x co-ordinate of the tile we are checking to move to
//The y co-ordinate of the tile we are checking to move to
//RETURNS true if the element was sucessful and added to the open tiles list
bool PathFinding::workoutPathwayDataToTargetTile(Level * level, Tile * finalDestination, Tile * currentSquare, vector<Tile*> &openTiles, int x, int y) {

	Tile * loopedNeighbour = level->TileGrid[y][x];

	//only work out the pathday data if we weren't cutting the corner of a wall
	if (PathFinding::checkIfCutCorner(currentSquare, loopedNeighbour, level) == true)
		return false;

	//check to see if its not on the open list already
	if (std::find(openTiles.begin(), openTiles.end(), loopedNeighbour) == openTiles.end()) {

		//set that parent tile to be that of the tile we're coming from
		loopedNeighbour->setPathfindingParent(currentSquare);
		PathFinding::setCostData(loopedNeighbour, finalDestination);

		//add it to the open list
		openTiles.push_back(loopedNeighbour);

		return true;
	}
	
	//else if on the openlist check to see if we have found a better pathway to the looped tile
	else {

		//if the previously calculated gcost is greater than what the gcost would be if moved to from currentSquare, then change the parent
		if (level->TileGrid[y][x]->GCost > currentSquare->GCost) {

			level->TileGrid[y][x]->setPathfindingParent(currentSquare);
			PathFinding::setCostData(level->TileGrid[y][x], finalDestination);
		}
	}
	

	return false;
}

//Sets the cost data for a tile according to if it was a diagonal move or not with;
//The tile that we want to calculate the cost for
//The destination tile for the path
void PathFinding::setCostData(Tile * theTile, Tile * finalDestination) {

	int xCost = abs(finalDestination->X - theTile->X);
	int yCost = abs(finalDestination->Y - theTile->Y);

	//guess the distance to the end tile
	int Hcost = (xCost + yCost) * 10;

	//get the accumulative distance for moving from the source of the path to this tile along the current parent heirarchy
	int Gcost = theTile->PathfindingParent->GCost;

	float exposureConst = 70;	//where increasing it lowers the effect of exposure
	if (theTile->Weighting != 0) {

		//accumilate the exposure
		theTile->Exposure = (theTile->Weighting / exposureConst) + theTile->PathfindingParent->Exposure;
	}
	else {

		//limit the exposure
		theTile->Exposure = theTile->PathfindingParent->Exposure / exposureConst;
	}

	//Infulence the cost of moving that tile by its weighting					//multiplied by a constant
	Gcost += (theTile->Weighting + theTile->PathfindingParent->Weighting) / 2 * 0.2f;
	//and its exposure to move to this tile
	Gcost += theTile->Exposure;

	//work out if this tile is a diagonal of the parent
	if (PathFinding::checkIfDiagonalMove(theTile, theTile->PathfindingParent) == true)
		//it is so add on the diagonal distance
		Gcost += 14;
	else
		//if not diagonal, just add on the length of the square
		Gcost += 10;

	//calculate the Fcosts etc for the tile
	theTile->assignCosts(Gcost, Hcost);
}

//Checks to see if by moving from the source to the destination in the map 'level' will be
//cutting across a tile that is impassable
bool PathFinding::checkIfCutCorner(Tile * source, Tile * destination, Level * level) {

	//the only way we could cut a corner is if its a diagonal move
	if (PathFinding::checkIfDiagonalMove(source, destination) == true) {

		//work out which tile (source or destination) is on the left and which is on the right
		Tile * leftMostTile = (source->X < destination->X ? source : destination);
		Tile * rightMostTile = (source->X > destination->X ? source : destination);

		//if true, the diagonal is going from the bottom left to the top right
		if (leftMostTile->Y > rightMostTile->Y) {

			//check either of the possible corners to see if we are cutting a corner
			if (leftMostTile->X + 1 < level->TilesWide) {

				Tile * corner1 = level->TileGrid[leftMostTile->Y][leftMostTile->X + 1];
				if (corner1->CoverType != kOpen)
					return true;
			}

			if (rightMostTile->X - 1 >= 0) {

				Tile * corner2 = level->TileGrid[rightMostTile->Y][rightMostTile->X - 1];
				if (corner2->CoverType != kOpen)
					return true;
			}
		}
		//else other diagonal is going from top right to bottom left
		else {

			//check either of the possible corners to see if we are cutting a corner
			if (leftMostTile->Y + 1 < level->TilesTall) {

				Tile * corner1 = level->TileGrid[leftMostTile->Y + 1][leftMostTile->X];
				if (corner1->CoverType != kOpen)
					return true;
			}

			if (rightMostTile->Y - 1 >= 0) {

				Tile * corner2 = level->TileGrid[rightMostTile->Y - 1][rightMostTile->X];
				if (corner2->CoverType != kOpen)
					return true;
			}
		}
	}

	//if we made it this far then we weren't cutting any corners
	return false;
}

//Checks to see if a move from the source tile to the destination tile is diagonal
bool PathFinding::checkIfDiagonalMove(Tile * source, Tile * destination) {

	//Iterate through the corner tiles of the source tile
	for (int x = source->X - 1; x < source->X + 2; x += 2) {
        for (int y = source->Y - 1; y < source->Y + 2; y += 2) {

            //if the destination tile is one that is on the diagonals
			if (destination->X == x && destination->Y == y)
                return true;
        }
    }

    //if we made it this far, then we are going in a straight line
    return false;
}

//Method for when we have found the destination tile along a path and we need to iterate back through it to get back to the source
//Also resets the closed and open tiles. Does operations with;
//The destination tile which we found
//reference to the pathway vector which will return all the positions we must move though
//reference to all the closed tiles we collected
//reference to all the open tiles wer clolected
void PathFinding::foundDestination(Tile * finalDestination, vector<Tile*> &pathway, vector<Tile*> &closedTiles, vector<Tile*> openTiles) {

	pathway.push_back(finalDestination);

	Tile * loopedTile = finalDestination;
	//loop through all the parents along the pathway until we reach the end
	do {
		loopedTile = loopedTile->PathfindingParent;
		pathway.push_back(loopedTile);
	}
	while (loopedTile->HasPathfindingParent == true);

	//reverse the vector of positions
	int a = 0;
	int b = pathway.size();

	while (a < b) {

		b--;
		Tile * tempPos = pathway[a];
		pathway[a] = pathway[b];
		pathway[b] = tempPos;
		a++;
	}

	//remove the first note from the pathway since this is where the player started at
	pathway.erase(pathway.begin());

	resetUsedTiles(closedTiles, openTiles);
}

//Resets all the tiles used in the level
void PathFinding::resetUsedTiles(vector<Tile*> &closedTiles, vector<Tile*> openTiles) {

	//reset all of the parent tiles so next time the algorthim runs, it is done fresh
	for (int i = 0; i < (int)closedTiles.size(); i++) {

		closedTiles[i]->resetTile();
	}

	for (int i = 0; i < (int)openTiles.size(); i++) {

		openTiles[i]->resetTile();
	}
}

//resets all of the tiles affected by the infulence data
void PathFinding::resetInfulencedTiles(vector<InfulenceData> levelInfulence) {

	for (int i = 0; i < (int)levelInfulence.size(); i++) {
		
		levelInfulence[i].TheTile->resetTile();
	}
}