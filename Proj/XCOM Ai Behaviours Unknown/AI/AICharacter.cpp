#include "AICharacter.h"

#include "Team.h"
#include "../Characters and Weapons/BaseWeapon.h"
#include "DecisionTree.h"
#include "ActionSequence.h"

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
void AICharacter::RegisterWeapon(BaseWeapon * weapon) {

	Weapon = weapon;
	weapon->ModifyPlayerStats(this);
}

//Registers this AI character to a given team
void AICharacter::RegisterTeam(Team * theTeam) {

	ParentTeam = theTeam;
	theTeam->AddTeamMember(this);
}

//Updates the player, its running actions, and its tile position
void AICharacter::Update(float dt) {

	//Reset the trait variables
	Boldness = _sBoldness;
	Aggression = _sAggression;
	Comradery = _sComradery;

	UpdateChildTraits();
	Weapon->ModifyPlayerStats(this);

	//update what tile position the player is said to be on
	UpdateTilePosition();
	UpdateTileHistory(dt);
	UpdateInfulence();

	//update the weapon's cooldowns etc
	Weapon->Update(dt);

	//update the characters current action or reanalyse its situation
	UpdateActions(dt);

	//reset modifiers
	AccuracyModifier = 1;
	MoveModifier = 1;
}

//Updates the characters actions and decision making
void AICharacter::UpdateActions(float dt) {

	if (_reactionCountdown > 0) {

		//if there is a close enemy to this character, increase their reaction time to give more updated actions
		float modifiedDt = dt;
		vector<AICharacter*> enemies = ParentTeam->GetVisibleEnemies();
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
		if (CurrentAct->IsActionComplete() == false)
			CurrentAct->Act(dt, this);

		//see if the action has just been finished
		if (CurrentAct->IsActionComplete() == true) {

			CurrentAct->Deleted();
			HasAction = false;
			delete CurrentAct;
		}
	}

	//if character doesn't have an action or if its time for it to reevaulate..
	if (HasAction == false || _reactionCountdown <= 0) {

		//get the new action
		Action * decidedAction = TheDecisionTree->Run();

		//reset the countdown
		_reactionCountdown = ReactionTime;

		//if it had no action, just set it as the current
		if (HasAction == false)
			SetAction(decidedAction);

		else if (CurrentAct->State != kCancelling && CurrentAct->CanInterrupt() == true) {

			//see if the new action should be ran over the previous action
			if (CurrentAct->ShouldGiveWayTo(decidedAction) == true) {

				//add the current action in a sequence while cancelling it
				CurrentAct->Cancel();

				//if the action was able to finish immediately, set the new action
				if (CurrentAct->IsActionComplete() == true)
					SetAction(decidedAction);
			}
		}
	}
}

//Updates what tile the current player is inteprted to be on,
//and moves them to the center of the tile if the character has no movement set
void AICharacter::UpdateTilePosition() {

	//update the tiles that the character is on
	if (CurrentTile != DestinationTile) {

		//if the character is closer to the destination than they are to the current tile, update the current tile var
		if (Position.distance(CurrentTile->Position) > Position.distance(DestinationTile->Position))
			MovedToTile(DestinationTile);
	}
}

//Updates the tile history of this character, removing any tiles that are beyond a threshold
void AICharacter::UpdateTileHistory(float dt) {

	//length of tile a tile needs to be stored for before it is deemed too old to be counted anymore
	int threshold = 10;

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
void AICharacter::MovedToTile(Tile * newTile) {

	CurrentTile = newTile;
}

//Updates the infulence
void AICharacter::UpdateInfulence() {

	//work out the new infulence map
	vector<Tile*> losTiles = CurrentTile->GetLosTiles();
	InfulencedTiles.clear();

	for (int i = 0; i < (int)losTiles.size(); i++) {

		Tile * loopedTile = losTiles[i];

		//cap the accuracy to 100
		float accuracy = Weapon->GetAccuracyToTarget(loopedTile->Position);

		if (loopedTile->IsInCoverFrom(Position))
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
bool AICharacter::IsInCoverFromAttack(Vector3 attackOrigin) {

	return CurrentTile->IsInCoverFrom(attackOrigin);
}

//Sets the AI to start performing an action.
void AICharacter::SetAction(Action * action) {

	HasAction = true;
	CurrentAct = action;

	//reset the renewel countdown
	_reactionCountdown = ReactionTime;
}

//Adds an attacker to the attackers vector
void AICharacter::RegisterAttack(AttackData data) {

	_attackers.push_back(data);

	if (data.AttackType == kSuppressing)
		IsSuppressed = true;
}

//Removes an attacker from the attackers vector
void AICharacter::UnRegisterAttacker(AICharacter * attacker) {

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

		CurrentAct->HardCancel();
		delete CurrentAct;
	}
}

//getters and setters for AI surroundings
//============================================================
vector<AttackData> AICharacter::GetAttackers() {

	return _attackers;
}
vector<TileHistory> AICharacter::GetTileHistory() {

	return _history;
}

//getters and setters for AI variables
//============================================================
void AICharacter::SetBaseAggression(int agg) {

	_sAggression = agg;
	KeepTraitToBounds(_sAggression, 0, 10);

	//adjust the current trait if all of a sudden the character is more than it should be
	if (Aggression > _sAggression)
		Aggression = _sAggression;
}

int AICharacter::GetBaseAggression() {

	return _sAggression;
}

void AICharacter::SetBaseBoldness(int bold) {

	_sBoldness = bold;
	KeepTraitToBounds(_sBoldness, 0, 10);

	//adjust the current trait if all of a sudden the character is more than it should be
	if (Boldness > _sBoldness)
		Boldness = _sBoldness;
}

int AICharacter::GetBaseBoldness() {

	return _sBoldness;
}

void AICharacter::SetBaseComradery(int comradery) {

	_sComradery = comradery;
	KeepTraitToBounds(_sComradery, 0, 10);

	//adjust the current trait if all of a sudden the character is more than it should be
	if (Comradery > _sComradery)
		Comradery = _sComradery;
}

int AICharacter::GetBaseComradery() {

	return _sComradery;
}
//============================================================

void AICharacter::KeepTraitToBounds(int &trait, int lowerBound, int upperBound) {

	if (trait < lowerBound)
		trait = lowerBound;

	if (trait > upperBound)
		trait = upperBound;
}