#ifndef BaseWeaponH
#define BaseWeaponH

#include <string>

#include "../Misc/aimath.h"
class AICharacter;

enum HitTypes
{ kMiss, kHit, kSuppress };

class BaseWeapon {

	//Variables
	//===================================================
public:
	float FireRate;	//The number of times this weapon can shoot per second
	float ReloadTime;	//The amount of time needed to reload the weapon
	bool IsReloading;	//Flag for whether the weapon is currently reloading
	float Range;	//The range that this weapon can engage targets at
	float Accuracy;	//The percentage chance that this weapon will hit per bullet fired, at point blanc
	float AccuracyDrop;	//The amount the accuracy that is reduced per percentage of max range
	int ClipSize;	//The number of bullets that are stored per magazine
	int BulletsPerShot;	//The number of bullets that are fired per shot
	float CoverPenalty;	//The division of accuracy if the target is behind cover
	float SuppressPenality;	//The division of a targets movement and accuracy

	AICharacter * Holder;	//The holder of this weapon

protected:
	int _shotsInClip;	//The number of shots that are left in the current clip

	float _fireCooldownTime;	//Time until the weapon can fire again
	float _reloadCooldownTime;	//Time until the weapon reload is complete

	HitTypes _shotType;	//enum for the type of shot that was fired
	bool _weaponFired;	//Flag for whether the weapon is firing
	Vector3 _targetPos;	//The position of the target the weapon is shooting at

	//Methods
	//===================================================
public:
	BaseWeapon::BaseWeapon();
	void shootAt(AICharacter * target);
	void suppressTarget(AICharacter * target);
	bool canShoot();
	bool isInRange(Vector3 position);
	void reload();
	float getAccuracyToTarget(Vector3 targetPos);
	float getAccuracyFromPlaceToTarget(Vector3 startPos, Vector3 targetPos);
	float getAccuracyToCharIncCover(AICharacter * target);
	float getAmmoRatio();
	void update(float dt);
	void draw();

	virtual std::string toString() = 0;
	virtual bool canSuppress() = 0;
	virtual void modifyPlayerStats(AICharacter * character) = 0;
};

#endif