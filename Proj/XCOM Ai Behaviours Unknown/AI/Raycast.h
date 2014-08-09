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
	static bool castRay(Tile * source, Tile *destination);
	static void visibleTiles(vector<Tile*> &visibleTiles, Tile * source);
};

#endif