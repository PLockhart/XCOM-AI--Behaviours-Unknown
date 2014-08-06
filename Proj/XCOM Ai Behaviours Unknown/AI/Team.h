#pragma once
#ifndef TeamH
#define TeamH

#include <vector>
class AICharacter;
struct InfulenceData;
struct TileHistory;
struct EnemyPositionHistory;
class Tile;

using namespace std;

class Team {

	//Variables
	//===================================================
public:
	int Identifier;	//The team number identifier
	int HistoryRecordLength;	//Length of time that an enemy history position is tracked
	vector<EnemyPositionHistory> EnemyPositionHist;	//List of positions that are/were occupied by enemies

private:
	vector<AICharacter*> _teamMembers;	//Members of this team
	vector<AICharacter*> _trackedEnemies;	//The enemies we know existand haven't died
	vector<AICharacter*> _visibleEnemies;	//The enemies that are visible to this team
	vector<AICharacter*> _prevVisibleEnemies;	//The previous frame's enemies that are visible to this team
	vector<Team*> _otherTeams;	//The other teams in the game

	vector<InfulenceData> _infulencedTiles;	//The tiles that this team collectively infulences
	vector<InfulenceData> _enemyInfulencedTiles;	//The tiles that the other teams collectively infulence
	vector<TileHistory> _teamHistory;	//The history of tiles that we have had visiion of
	vector<Tile*> _teamLOS;	//The tiles this team can see

	//flags
	bool _infulenceCalculated;	//Flag for whether the infulence data for this team was calculated this frame
	bool _enemyInfulenceCalculated;	//Flag for whether the infulence data for the other teams were calculated this frame
	bool _teamHistoryCalculated;	//Flag for whether the team's tile history has been calculated this frame
	bool _teamLOSCalculated;	//Flag for fwhether this team's LOS has been calculated

	//Methods
	//===================================================
public:
	//creations
	Team::Team(int identifier);
	vector<AICharacter*> GetTeammatesOf(AICharacter * character);
	void RegisterTeams(vector<Team*> teams);
	void AddTeamMember(AICharacter * member);

	//updating
	void Update(float dt);
	void RemoveMember(AICharacter * member);

	//gets
	vector<InfulenceData> GetInfulencedTiles();
	vector<InfulenceData> GetAssumedEnemyInfulencedTiles();
	vector<TileHistory> GetTeamTileHistory();
	vector<AICharacter*> GetVisibleEnemies();
	vector<AICharacter*> GetTrackedEnemies();
	vector<Tile*> GetTeamsVisibleTiles();

	void Draw();

private:
	//updating
	void SetVisibleEnemies();
	void UpdateEnemyPositionHistory(float dt);
	void UpdateTrackedEnemies();
};
#endif