#ifndef RaycastH
#define RaycastH

#include <vector>

#include "../Misc/aimath.h"
class Tile;
class Level;

using namespace std;

class Raycast {

	//Methods
	//===================================================
public:
	static bool CastRay(Tile * source, Tile *destination);
	static void VisibleTiles(vector<Tile*> &visibleTiles, Tile * source);
};

#endif