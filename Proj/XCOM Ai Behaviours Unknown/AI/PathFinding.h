#ifndef PathFindingH
#define PathFindingH

#include <vector>

#include "../Containers/DataContainers.h"
#include "../Misc/aimath.h"
class Level;
class Tile;

using namespace std;

class PathFinding {

	//Methods
	//===================================================
public:
	static bool getPathway(vector<Tile*> &pathway, Tile * start, Tile * destination);
	static bool getTacticalPathway(vector<InfulenceData> levelInfulence, vector<Tile*> &pathway, Tile * start, Tile * destination);
	static void floodMap(vector<Tile*> &avaliableTiles, Tile * start, int maxMoves);

private:
	//utilities used by the getPathway method and its method calls
	static void foundDestination(Tile * finalDestination, vector<Tile*> &pathway, vector<Tile*> &closedTiles, vector<Tile*> openTiles); 
	static bool workoutPathwayDataToTargetTile(Level * level, Tile * finalDestination, Tile * currentSquare, vector<Tile*> &openTiles, int x, int y);
	static bool checkIfCutCorner(Tile * source, Tile * destination, Level * level);
	static bool checkIfDiagonalMove(Tile * source, Tile * destination);
	static void setCostData(Tile * theTile, Tile * finalDestination);
	static void resetUsedTiles(vector<Tile*> &closedTiles, vector<Tile*> openTiles);
	static void resetInfulencedTiles(vector<InfulenceData> levelInfulence);
};

#endif