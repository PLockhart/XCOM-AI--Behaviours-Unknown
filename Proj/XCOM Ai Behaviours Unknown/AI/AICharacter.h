#ifndef AICharacterH
#define AICharacterH

#include <vector>

#include "../Misc/aimath.h"
#include "../Containers/DataContainers.h"
#include "Action.h"
#include "../Level/Level.h"
class Tile;
class BaseWeapon;
class AICharacter;
class Team;
class DecisionTree;

using namespace std;

//an ai component to be subclassed by characters wishing to use AI
class AICharacter {

	//Variables
	//===================================================
public:
	Vector3 Position;	//Position of ai character
	float Rotation;	//rotation of ai character in degrees
	float Speed;	//speed which character moves at
	float MoveModifier;	//Modifier to the movement/rot speeds, 1 is normal
	float RotSpeed;	//speed which the player rotates at
	bool IsSuppressed;	//Flag for whether this character is suppressed
	bool IsDead;	//Flag for whether this character is dead

	BaseWeapon * Weapon;	//The weapon the character uses
	float AccuracyModifier;	//Modifier to the firing accuracy, 1 is normal

	Team * ParentTeam;	//The team that this character belongs to
	vector<InfulenceData> InfulencedTiles;	//The tiles that this character infulences
	vector<AICharacter*> VisibleEnemies;	//The enemies that this character can see

	//AI vars
	int Boldness;	//The rating of whether a character will disgard cover etc
	int Aggression;	//The rating of how confrontational a character is in combat
	int Comradery;	//The rating of how much we want to help team mates
	float ReactionTime;	//The number of seconds that pass before a character reanalysises their situation
	DecisionTree * TheDecisionTree;	//The decision tree used to work out what this ai shall do

	bool HasAction;	//Flag for weither the character has an action
	Action * CurrentAct;	//The current action for the AI to perform
	Tile * CurrentTile;	//The current tile the ai is on
	Tile * DestinationTile;	//The tile the ai wants to move to

	bool Debug;	//a flag to help identify this character in the debugger

protected:
	//SCALE 0 TO 10
	int _sAggression;	//The unmodified aggression of the character
	int _sBoldness;	//The unmodified boldness of the character
	int _sComradery;	//The unmodified comradery of the character
	float _reactionCountdown;	//Time until character can reevaluate their situation

	vector<AttackData> _attackers;	//The people who are attacking this character
	vector<TileHistory> _history;	//The history of tiles we have viewed and when we viewed it

	//Methods
	//===================================================
public:
	AICharacter::AICharacter();
	void RegisterTeam(Team * theTeam);

	//updates
	void UpdateActions(float dt);
	virtual void MovedToTile(Tile * newTile);
	virtual void Update(float dt);
	void RegisterAttack(AttackData data);
	void UnRegisterAttacker(AICharacter * attacker);

	//actions
	virtual void SetAction(Action * action);

	//checks
	bool IsInCoverFromAttack(Vector3 attackOrigin);

	//gets and setters for data on surroundings
	vector<AttackData> GetAttackers();
	vector<TileHistory> GetTileHistory();

	//getters and setters for AI variables
	void SetBaseAggression(int agg);
	int GetBaseAggression();
	void SetBaseBoldness(int bold);
	int GetBaseBoldness();
	void SetBaseComradery(int comradery);
	int GetBaseComradery();

	//virtual methods that the subclassing component must implment
	virtual void MoveBy(Vector3 move) = 0;
	virtual void RotateBy(float amount) = 0;
	virtual void FaceTowards(Vector3 point) = 0;
	virtual void DamageTaken(AICharacter * source) = 0;
	
	virtual AICharacter::~AICharacter();

protected:
	virtual void UpdateChildTraits() = 0;
	void RegisterWeapon(BaseWeapon * weapon);
	void UpdateTilePosition();
	void UpdateTileHistory(float dt);
	void UpdateInfulence();
	void KeepTraitToBounds(int &trait, int lowerBound, int upperBound);
};

#endif