#ifndef DataContainersH
#define DataContainersH

class Tile;
class AICharacter;

struct InfulenceData {

	//Variables
	//===================================================
public:
	Tile * TheTile;	//Tile this infulence set is refering to
	float Infulence;	//The infulence on the tile

	//Methods
	//===================================================
public:
	InfulenceData::InfulenceData(Tile * tile, float infulence);
};

enum AttackTypes 
{ kBasic, kSuppressing };

struct AttackData {

	//Variables
	//===================================================
public:
	AICharacter * Attacker;	//The character performing the attack
	AttackTypes AttackType;	//The type of attack that is being formed

	//Methods
	//===================================================
public:
	AttackData::AttackData(AICharacter * attacker, AttackTypes attackType);
};

struct TileHistory {

	//Variables
	//===================================================
public:
	Tile * TheTile;	//The tile this data is refering to
	float Time;	//Range for the length of time since this tile has been travelled to. 0 is in sight, 1 is old

	//Methods
	//===================================================
	TileHistory::TileHistory(Tile * tile);
};

struct EnemyPositionHistory {

	//Variables
	//===================================================
public:
	Tile * TheTile;	//The tile this data is refering to
	AICharacter * TheEnemy;	//The enemy that was on the current tile
	float Time;	//Length of time since this position was looked at to check if an enemy was there

	//Methods
	//===================================================
	EnemyPositionHistory::EnemyPositionHistory(AICharacter * enemy);
};

#endif