#include "AICharacter.h"

#include "Team.h"
#include "../Characters and Weapons/BaseWeapon.h"
#include "DecisionTree.h"
#include "ActionSequence.h"

//length of time an enemy's tile position needs to be stored for before it is deemed too old to be counted anymore
const float CHAR_TILE_HISTORY_TIME = 10;

//Base class for an AI character
AICharacter::AICharacter() { 

	HasAction = false;
	Aggression = _sAggression = 3;
	Boldness = _sBoldness = 5;
	Comradery = _sComradery = 5;
	ReactionTime = _reactionCountdown = 1.4f;
	Rotation = 0;
	Speed = 2;
	RotSpeed = 8;
	MoveModifier = 1;
	AccuracyModifier = 1;
	IsSuppressed = false;
	IsDead = false;

	Debug = false;
}

//Registers the weapon being held and adjusts the ai character's stats
void AICharacter::registerWeapon(BaseWeapon * weapon) {

	Weapon = weapon;
	weapon->modifyPlayerStats(this);
}

//Registers this AI character to a given team
void AICharacter::registerTeam(Team * theTeam) {

	ParentTeam = theTeam;
	theTeam->addTeamMember(this);
}

//Updates the player, its running actions, and its tile position
void AICharacter::update(float dt) {

	//Reset the trait variables
	Boldness = _sBoldness;
	Aggression = _sAggression;
	Comradery = _sComradery;

	updateChildTraits();
	Weapon->modifyPlayerStats(this);

	//update what tile position the player is said to be on
	updateTilePosition();
	updateTileHistory(dt);
	updateInfulence();

	//update the weapon's cooldowns etc
	Weapon->update(dt);

	//update the characters current action or reanalyse its situation
	updateActions(dt);

	//reset modifiers
	AccuracyModifier = 1;
	MoveModifier = 1;
}

//Updates the characters actions and decision making
void AICharacter::updateActions(float dt) {

	if (_reactionCountdown > 0) {

		//if there is a close enemy to this character, increase their reaction time to give more updated actions
		float modifiedDt = dt;
		vector<AICharacter*> enemies = ParentTeam->getVisibleEnemies();
		for (int i = 0; i < (int)enemies.size(); i++) {

			if (enemies[i]->Position.distance(Position) <= 160) {

				modifiedDt *= 5;
				break;
			}
		}

		//surpressed units react at reduced speed
		if (IsSuppressed == true)
			modifiedDt /= 1.5f;

		_reactionCountdown -= modifiedDt;
	}
	
	//perform an action if this ai has none
	if (HasAction == true) {

		//check if the action isn't finished yet
		if (CurrentAct->isActionComplete() == false)
			CurrentAct->act(dt, this);

		//see if the action has just been finished
		if (CurrentAct->isActionComplete() == true) {

			CurrentAct->deleted();
			HasAction = false;
			delete CurrentAct;
		}
	}

	//if character doesn't have an action or if its time for it to reevaulate..
	if (HasAction == false || _reactionCountdown <= 0) {

		//get the new action
		Action * decidedAction = TheDecisionTree->run();

		//reset the countdown
		_reactionCountdown = ReactionTime;

		//if it had no action, just set it as the current
		if (HasAction == false)
			setAction(decidedAction);

		else if (CurrentAct->State != kCancelling && CurrentAct->canInterrupt() == true) {

			//see if the new action should be ran over the previous action
			if (CurrentAct->shouldGiveWayTo(decidedAction) == true) {

				//add the current action in a sequence while cancelling it
				CurrentAct->cancel();

				//if the action was able to finish immediately, set the new action
				if (CurrentAct->isActionComplete() == true)
					setAction(decidedAction);
			}
		}
	}
}

//Updates what tile the current player is inteprted to be on,
//and moves them to the center of the tile if the character has no movement set
void AICharacter::updateTilePosition() {

	//update the tiles that the character is on
	if (CurrentTile != DestinationTile) {

		//if the character is closer to the destination than they are to the current tile, update the current tile var
		if (Position.distance(CurrentTile->Position) > Position.distance(DestinationTile->Position))
			movedToTile(DestinationTile);
	}
}

//Updates the tile history of this character, removing any tiles that are beyond a threshold
void AICharacter::updateTileHistory(float dt) {

	//length of time a tile needs to be stored for before it is deemed too old to be counted anymore
	int threshold = CHAR_TILE_HISTORY_TIME;

	//firstly increase all the time's for the previously recorded history times, removing old ones
	for (int hist = 0; hist < (int)_history.size(); hist++) { 

		_history[hist].Time += dt / threshold;

		if (_history[hist].Time >= 1) {
			//remove the entry and reduce the loop index to avoid value skipping
			_history.erase(_history.begin() + hist);
			hist--;
		}
	}

	//loop through all the tiles we are currently infulencing
	for (int inf = 0; inf < (int)InfulencedTiles.size(); inf++) {

		bool prevRecord = false;

		//looping through all of the tiles that that we have previously stored
		for (int hist = 0; hist < (int)_history.size(); hist++) {

			if (_history[hist].TheTile == InfulencedTiles[inf].TheTile) {

				prevRecord = true;
				//reset the time since we last saw it
				_history[hist].Time = 0;
			}
		}

		//if we are looking at a new tile, create a new record for it
		if (prevRecord == false)
			_history.push_back(TileHistory(InfulencedTiles[inf].TheTile));
	}
}

//Method for when a character has moved to a different tile
void AICharacter::movedToTile(Tile * newTile) {

	CurrentTile = newTile;
}

//Updates the infulence
void AICharacter::updateInfulence() {

	//work out the new infulence map
	vector<Tile*> losTiles = CurrentTile->getLosTiles();
	InfulencedTiles.clear();

	for (int i = 0; i < (int)losTiles.size(); i++) {

		Tile * loopedTile = losTiles[i];

		//cap the accuracy to 100
		float accuracy = Weapon->getAccuracyToTarget(loopedTile->Position);

		if (loopedTile->isInCoverFrom(Position))
			accuracy /= Weapon->CoverPenalty;

		if (accuracy > 100)
			accuracy = 100;

		//multiply the accuracy by the weapons power to get the infulence
		float infulence = accuracy * Weapon->BulletsPerShot;

		if (infulence > 0)
			InfulencedTiles.push_back(InfulenceData(loopedTile, infulence));
	}
}

//Method that returns true if this character is in cover from an attack originating from a position
bool AICharacter::isInCoverFromAttack(Vector3 attackOrigin) {

	return CurrentTile->isInCoverFrom(attackOrigin);
}

//Sets the AI to start performing an action.
void AICharacter::setAction(Action * action) {

	HasAction = true;
	CurrentAct = action;

	//reset the renewel countdown
	_reactionCountdown = ReactionTime;
}

//Adds an attacker to the attackers vector
void AICharacter::registerAttack(AttackData data) {

	_attackers.push_back(data);

	if (data.AttackType == kSuppressing)
		IsSuppressed = true;
}

//Removes an attacker from the attackers vector
void AICharacter::unRegisterAttacker(AICharacter * attacker) {

	for (int i = 0; i < (int)_attackers.size(); i++) {

		if (_attackers[i].Attacker == attacker) {

			_attackers.erase(_attackers.begin() + i);
			break;
		}
	}

	//if the attack was a suppressing attack, see if we are no longer suppressed
	bool stillSuppressed = false;
	for (int i = 0; i < (int)_attackers.size(); i++) {

		if (_attackers[i].AttackType == kSuppressing)
			stillSuppressed = true;
	}

	if (stillSuppressed == false) {

		IsSuppressed = false;
		AccuracyModifier = 1;
		MoveModifier = 1;
	}
}

AICharacter::~AICharacter() {

	delete TheDecisionTree;
	delete Weapon;

	if (HasAction == true) {

		CurrentAct->hardCancel();
		delete CurrentAct;
	}
}

//getters and setters for AI surroundings
//============================================================
vector<AttackData> AICharacter::getAttackers() {

	return _attackers;
}
vector<TileHistory> AICharacter::getTileHistory() {

	return _history;
}

//getters and setters for AI variables
//============================================================
void AICharacter::setBaseAggression(int agg) {

	_sAggression = agg;
	keepTraitToBounds(_sAggression, 0, 10);

	//adjust the current trait if all of a sudden the character is more than it should be
	if (Aggression > _sAggression)
		Aggression = _sAggression;
}

int AICharacter::getBaseAggression() {

	return _sAggression;
}

void AICharacter::setBaseBoldness(int bold) {

	_sBoldness = bold;
	keepTraitToBounds(_sBoldness, 0, 10);

	//adjust the current trait if all of a sudden the character is more than it should be
	if (Boldness > _sBoldness)
		Boldness = _sBoldness;
}

int AICharacter::getBaseBoldness() {

	return _sBoldness;
}

void AICharacter::setBaseComradery(int comradery) {

	_sComradery = comradery;
	keepTraitToBounds(_sComradery, 0, 10);

	//adjust the current trait if all of a sudden the character is more than it should be
	if (Comradery > _sComradery)
		Comradery = _sComradery;
}

int AICharacter::getBaseComradery() {

	return _sComradery;
}
//============================================================

void AICharacter::keepTraitToBounds(int &trait, int lowerBound, int upperBound) {

	if (trait < lowerBound)
		trait = lowerBound;

	if (trait > upperBound)
		trait = upperBound;
}