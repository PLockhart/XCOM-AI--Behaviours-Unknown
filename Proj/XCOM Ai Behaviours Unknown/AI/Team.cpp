#include <algorithm>

#include "Team.h"
#include "AICharacter.h"
#include "Raycast.h"
#include "../Level/Tile.h"
#include "../Containers/DataContainers.h"
#include "../Characters and Weapons/Character.h"

//Length of time that an enemy's previous position should be tracked for. Increasing this decreases performance but will make AI remember enemy's move patterns better
const int HISTORY_RECORD_LENGTH_CONST = 10;

//Creates a new team with an identifier
Team::Team(int num) {

	_infulenceCalculated = false;
	Identifier = num;
	HistoryRecordLength = HISTORY_RECORD_LENGTH_CONST;
}

//Gets all the team mates of the provided character
vector<AICharacter*> Team::getTeammatesOf(AICharacter * character) {

	vector<AICharacter*> teamMates;

	for (int i = 0; i < (int)_teamMembers.size(); i++) {

		if (_teamMembers[i] != character)
			teamMates.push_back(_teamMembers[i]);
	}

	return teamMates;
}

//registers all the teams in the list, ignoring itself if present in the vector
void Team::registerTeams(vector<Team*> teams) {

	_otherTeams.clear();

	for (int i = 0; i < (int)teams.size(); i++) {

		if (teams[i] != this)
			_otherTeams.push_back(teams[i]);
	}
}

//Adds a character to the team and sets that character's team to this instance
void Team::addTeamMember(AICharacter * member) {

	//if not already added to the team, add it
	if (std::find(_teamMembers.begin(), _teamMembers.end(), member) == _teamMembers.end()) {

		_teamMembers.push_back(member);
		member->ParentTeam = this;
	}
}

//Updates this team and all of its characters
void Team::update(float dt) {

	_infulenceCalculated = false;
	_enemyInfulenceCalculated = false;
	_teamHistoryCalculated = false;
	_teamLOSCalculated = false;

	//update the information relating to enemies to this team 
	setVisibleEnemies();
	updateTrackedEnemies();
	updateEnemyPositionHistory(dt);
}

//Returns all the tiles that this team can see
vector<Tile*> Team::getTeamsVisibleTiles() {

	if (_teamLOSCalculated == true)
		return _teamLOS;

	_teamLOS.clear();

	//loop through all the team members
	for (int i = 0; i < (int)_teamMembers.size(); i++) {

		//getting thier line of sight
		vector<Tile*> loopedLos = _teamMembers[i]->CurrentTile->getLosTiles();

		//loop all the entries against the teams LOS and avoide duplicates, while adding new entries
		for (int j = 0; j < (int)loopedLos.size(); j++) {

			Tile * loopedTile = loopedLos[j];

			//if not in the team LOS, add it
			if (std::find(_teamLOS.begin(), _teamLOS.end(), loopedTile) == _teamLOS.end())
				_teamLOS.push_back(loopedTile);
		}

		//add the current tile the team member is on if not part of the last
		if (std::find(_teamLOS.begin(), _teamLOS.end(), _teamMembers[i]->CurrentTile) == _teamLOS.end())
				_teamLOS.push_back(_teamMembers[i]->CurrentTile);
	}

	_teamLOSCalculated = true;
	return _teamLOS;
}

//Updates all of the enemy position histories
void Team::updateEnemyPositionHistory(float dt) {

	vector<Tile*> teamLos = getTeamsVisibleTiles();

	//loop through all the position histories and update them
	for (int i = 0; i < (int)EnemyPositionHist.size(); i++) {

		EnemyPositionHist[i].Time += dt;

		//if the threshold has been surpassed... ,
		if (EnemyPositionHist[i].Time >= HistoryRecordLength || 
			//the enemy is dead....,
			EnemyPositionHist[i].TheEnemy->IsDead == true ||
			//..we can now see this tile through a team mate..
			std::find(teamLos.begin(), teamLos.end(), EnemyPositionHist[i].TheTile) != teamLos.end()) {

			//..then remove the record
			EnemyPositionHist.erase(EnemyPositionHist.begin() + i);
			i--;
			continue;
		}
	}

	//loop through all the visible enemies, refreshing their position history
	//and removing any previous entries of them as they are now out of date
	vector<AICharacter*> visEnems = getVisibleEnemies();
	for (int visEnmIndx = 0; visEnmIndx < (int)visEnems.size(); visEnmIndx++) {

		AICharacter * loopedEnemy = visEnems[visEnmIndx];

		//flag for whether this enemy has an up to date record in the history list
		bool prevRecord = false;

		for (int enmHistIndx = 0; enmHistIndx < (int)EnemyPositionHist.size(); enmHistIndx++) {

			//if the looped enemy has already been recorded in the history....
			if (EnemyPositionHist[enmHistIndx].TheEnemy == loopedEnemy) {

				//if it was recording what is now an outdated postion, then remove it
				if (EnemyPositionHist[enmHistIndx].TheTile != loopedEnemy->CurrentTile) {

					EnemyPositionHist.erase(EnemyPositionHist.begin() + enmHistIndx);
					enmHistIndx--;
					continue;
				}

				//else if the history positin is still accurage, reset the time
				else {

					EnemyPositionHist[enmHistIndx].Time = 0;
					prevRecord = true;
				}
			}
		}

		//if no previous record for this enemy has been recorded, create one
		if (prevRecord == false) {

			EnemyPositionHistory newRecord = EnemyPositionHistory(loopedEnemy);
			EnemyPositionHist.push_back(newRecord);
		}
	}

	//compile a list of all the enemies that were visible last frame but are no longer visible
	vector<AICharacter*> disappearedEnemies;
	for (int i = 0; i < (int)_prevVisibleEnemies.size(); i++) {

		bool stillVisible = false;

		for (int j = 0; j < (int) _visibleEnemies.size(); j++) {

			if (_visibleEnemies[j] == _prevVisibleEnemies[i]) {

				stillVisible = true;
				return;
			}
		}

		if (stillVisible == false)
			disappearedEnemies.push_back(_prevVisibleEnemies[i]);
	}

	//create a new record history for all the times that are no invisible
	//where the record will be for the tile we last saw them moving to (ie their now current tile)
	for (int i = 0; i < (int)disappearedEnemies.size(); i++) {

		for (int j = 0; j < (int)EnemyPositionHist.size(); j++) {

			//remove any history references to the disappeared enemy
			if (EnemyPositionHist[j].TheEnemy == disappearedEnemies[i]) {

				EnemyPositionHist.erase(EnemyPositionHist.begin() + j);
				j--;
			}
		}

		EnemyPositionHistory newRecord = EnemyPositionHistory(disappearedEnemies[i]);
		EnemyPositionHist.push_back(newRecord);
	}
}

//Removes a team member from the team
void Team::removeMember(AICharacter * member) {

	_teamMembers.erase(std::remove(_teamMembers.begin(), _teamMembers.end(), member));
}

//returns all of the tiles infulenced by this team
vector<InfulenceData> Team::getInfulencedTiles() {

	if (_infulenceCalculated == true)
		return _infulencedTiles;

	_infulencedTiles.clear();

	//loop through all the team members
	for (int memNum = 0; memNum < (int)_teamMembers.size(); memNum++) {

		vector<InfulenceData> membersInfulence = _teamMembers[memNum]->InfulencedTiles;

		//loop through all of the infulenced tiles of this character
		for (int memberInf = 0; memberInf < (int)membersInfulence.size(); memberInf++) {

			InfulenceData loopedMember = membersInfulence[memberInf];
			bool prevRecordExist = false;

			//loop through all of the tiles in the teams infulenced tiles
			for (int teamInf = 0; teamInf < (int)_infulencedTiles.size(); teamInf++) {

				InfulenceData loopedTeam = _infulencedTiles[teamInf];

				//if we have overlapping infulence tiles, accumilate them
				if (loopedTeam.TheTile == loopedMember.TheTile) {

					_infulencedTiles[teamInf].Infulence += loopedMember.Infulence;
					prevRecordExist = true;
					break;
				}
			}

			//if there wasn't a previous record of this tile infulence, then add it as a new one to the team infulence
			if (prevRecordExist == false)
				_infulencedTiles.push_back(loopedMember);
		}
	}

	_infulenceCalculated = true;
	return _infulencedTiles;
}

//returns all of the tiles that are infulenced by the other enemy teams, from the team mates we can see
vector<InfulenceData> Team::getAssumedEnemyInfulencedTiles() {

	if (_enemyInfulenceCalculated == true)
		return _enemyInfulencedTiles;

	_enemyInfulencedTiles.clear();

	//loop through all the visible enemies
	for (int enemyIndx = 0; enemyIndx < (int)_visibleEnemies.size(); enemyIndx++) {

		//get the looped enemies infulenced tiles
		vector<InfulenceData> enemyInfulence = _visibleEnemies[enemyIndx]->InfulencedTiles;

		//loop through all of the infulenced tiles by that team
		for (int enemyInfulIndx = 0; enemyInfulIndx < (int)enemyInfulence.size(); enemyInfulIndx++) {

			InfulenceData enemyInfulencedTile = enemyInfulence[enemyInfulIndx];
			bool newEntry = true;

			//looping through all of the tiles from all the enemy teams so far...
			for (int teamInfulIndx = 0; teamInfulIndx < (int)_enemyInfulencedTiles.size(); teamInfulIndx++) {

				InfulenceData loopedInfulenceRecord = _enemyInfulencedTiles[teamInfulIndx];

				//if we have overlapping infulence tiles, accumilate them
				if (enemyInfulencedTile.TheTile == loopedInfulenceRecord.TheTile) {

					_enemyInfulencedTiles[teamInfulIndx].Infulence += enemyInfulencedTile.Infulence;
					newEntry = false;
					break;
				}
			}

			//if we didn't find a previous record of it in the system then add i
			if (newEntry == true)
				_enemyInfulencedTiles.push_back(enemyInfulencedTile);
		}
	}

	_enemyInfulenceCalculated = true;
	return _enemyInfulencedTiles;
}

//Returns all the tile history that each memeber of this team has seen
vector<TileHistory> Team::getTeamTileHistory() {

	if (_teamHistoryCalculated == true)
		return _teamHistory;

	_teamHistory.clear();

	//loop through all the team mates
	for (int teamMem = 0; teamMem < (int)_teamMembers.size(); teamMem++) {

		vector<TileHistory> memberVision = _teamMembers[teamMem]->getTileHistory();

		//looping through all of the tiles from the team member
		for (int membHist = 0; membHist < (int)memberVision.size(); membHist++) {

			bool prevRecordExists = false;
			TileHistory memberHistory = memberVision[membHist];

			//loop through all of the tile history we have gathered so far and see if we have any duplicates
			for (int teamHist = 0; teamHist < (int)_teamHistory.size(); teamHist++) {

				TileHistory teamHistory = _teamHistory[teamHist];

				//if already recorded, choose the youngest one
				if (memberHistory.TheTile == teamHistory.TheTile) {

					_teamHistory[teamHist].Time = (memberHistory.Time < teamHistory.Time ? memberHistory.Time : teamHistory.Time);
					prevRecordExists = true;
				}
			}

			if (prevRecordExists == false)
				_teamHistory.push_back(memberHistory);
		}
	}

	_teamHistoryCalculated = true;
	return _teamHistory;
}

//Finds all of the enemies that are visible and adds them to the visible enemies vector
void Team::setVisibleEnemies() {

	_prevVisibleEnemies = _visibleEnemies;
	_visibleEnemies.clear();

	//loop through all of the teams members..
	for (int memberIndex = 0; memberIndex < (int)_teamMembers.size(); memberIndex++) {

		AICharacter * teamMember = _teamMembers[memberIndex];
		teamMember->VisibleEnemies.clear();

		//going through each of the enemy teams...
		for (int teamIndex = 0; teamIndex < (int)_otherTeams.size(); teamIndex++) {

			Team* loopedTeam = _otherTeams[teamIndex];
			int numTeam = (int)loopedTeam->_teamMembers.size();

			//looping through all of their team members...
			for (int enemyIndex = 0; enemyIndex < numTeam; enemyIndex++) {

				AICharacter * loopedEnemy = loopedTeam->_teamMembers[enemyIndex];

				//to see if we can see them
				if (Raycast::castRay(teamMember->CurrentTile, loopedEnemy->CurrentTile) == true) {

					//add it to the looped characters vector of visible enemies
					teamMember->VisibleEnemies.push_back(loopedEnemy);

					//if it wasn't in the team's visible enemies vector, add it
					if (std::find(_visibleEnemies.begin(), _visibleEnemies.end(), loopedEnemy) ==_visibleEnemies.end())
						_visibleEnemies.push_back(loopedEnemy);
				}
			}
		}
	}
}

//Updates the enemies that this team has encountered, and are still alive
void Team::updateTrackedEnemies() {

	//looping through all of the visible enemies
	for (int i = 0; i < (int)_visibleEnemies.size(); i++) {

		//check to see if we have encountered an untracked enemy
		if (std::find(_trackedEnemies.begin(), _trackedEnemies.end(), _visibleEnemies[i]) == _trackedEnemies.end()) {

			//add it to the tracked enemies
			_trackedEnemies.push_back(_visibleEnemies[i]);
		}

		//else if it already exists, remove it if the enemy is now dead
		else if (_visibleEnemies[i]->IsDead == true)
			std::remove(_trackedEnemies.begin(), _trackedEnemies.end(), _visibleEnemies[i]);
	}
}

//Returns all the visible enemies to this team
vector<AICharacter*> Team::getVisibleEnemies() {

	return _visibleEnemies;
}

//Returns all the enemies this teams know are alive
vector<AICharacter*> Team::getTrackedEnemies() {

	return _trackedEnemies;
}

//Draws characters from this teams perspective. Not a proper implementation, doesn't belong to team class
//for demonstrational purposes only
void Team::draw() {

	for (int i = 0; i < (int)_teamMembers.size(); i++) {

		Character * derived = dynamic_cast<Character*>(_teamMembers[i]);

		derived->draw();
	}

	for (int i = 0; i < (int)_visibleEnemies.size(); i++) {

		Character * derived = dynamic_cast<Character*>(_visibleEnemies[i]);

		derived->draw();
	}
}