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
	static bool GetPathway(vector<Tile*> &pathway, Tile * start, Tile * destination);
	static bool GetTacticalPathway(vector<InfulenceData> levelInfulence, vector<Tile*> &pathway, Tile * start, Tile * destination);
	static void FloodMap(vector<Tile*> &avaliableTiles, Tile * start, int maxMoves);

private:
	//utilities used by the GetPathway method and its method calls
	static void FoundDestination(Tile * finalDestination, vector<Tile*> &pathway, vector<Tile*> &closedTiles, vector<Tile*> openTiles); 
	static bool WorkoutPathwayDataToTargetTile(Level * level, Tile * finalDestination, Tile * currentSquare, vector<Tile*> &openTiles, int x, int y);
	static bool CheckIfCutCorner(Tile * source, Tile * destination, Level * level);
	static bool CheckIfDiagonalMove(Tile * source, Tile * destination);
	static void SetCostData(Tile * theTile, Tile * finalDestination);
	static void ResetUsedTiles(vector<Tile*> &closedTiles, vector<Tile*> openTiles);
	static void ResetInfulencedTiles(vector<InfulenceData> levelInfulence);
};

#endif