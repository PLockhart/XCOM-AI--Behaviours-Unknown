#include "DataContainers.h"

#include "../Level/Tile.h"
#include "../AI/AICharacter.h"

//infulence data stores the amount of infulence a character or team has over a tile
//where infulence is the targets accuracy to a tile * the damage they can do
InfulenceData::InfulenceData(Tile * tile, float infulence) {

	TheTile = tile;
	Infulence = infulence;
}

//attackdata stores an attacking character and what type of attack they are performing
AttackData::AttackData(AICharacter * attacker, AttackTypes attackType) {

	Attacker = attacker;
	AttackType = attackType;
}

//tile history represents tiles that we have explored and the timescale since we saw them, from 0 new to 1 forgotten
TileHistory::TileHistory(Tile * tile) {

	Time = 0;
	TheTile = tile;
}

//represents a position that we last saw an enemy at
EnemyPositionHistory::EnemyPositionHistory(AICharacter * enemy) {

	Time = 0;
	TheTile = enemy->CurrentTile;
	TheEnemy = enemy;
}