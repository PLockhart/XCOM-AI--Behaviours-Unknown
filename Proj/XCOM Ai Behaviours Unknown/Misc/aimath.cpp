/*
 * Part of the Artificial Intelligence for Games system.
 *
 * Copyright (c) Ian Millington 2003-2006. All Rights Reserved.
 *
 * This software is distributed under licence. Use of this software
 * implies agreement with all terms and conditions of the accompanying
 * software licence.
 */

#include "aimath.h"


    const Vector3 Vector3::UP = Vector3(0,1,0);
    const Vector3 Vector3::RIGHT = Vector3(1,0,0);
    const Vector3 Vector3::OUT_OF_SCREEN = Vector3(0,0,1);
    const Vector3 Vector3::DOWN = Vector3(0,-1,0);
    const Vector3 Vector3::LEFT = Vector3(-1,0,0);
    const Vector3 Vector3::INTO_SCREEN = Vector3(0,0,-1);
    const Vector3 Vector3::X = Vector3(1,0,0);
    const Vector3 Vector3::Y = Vector3(0,1,0);
    const Vector3 Vector3::Z = Vector3(0,0,1);


	void Rotations::ClampDegreeRotation(float &rotation) {

		if (rotation < 0) {

			rotation = 360 + rotation;
		}

		else if (rotation > 360) {

			rotation = (float)((int)rotation % 360);
		}

		if (rotation == 360)
			rotation = 0;
	}

	//assumes degrees
	bool Rotations::RotationsSimilair(float rot1, float rot2, float leeway) {

		Rotations::ClampDegreeRotation(rot1);
		Rotations::ClampDegreeRotation(rot2);

		int difference = (int)(rot1 - rot2);

		if (abs(difference % 360) <= leeway  || 360 - abs(difference % 360) < leeway)
			return true;

		return false;
	}

	//Returns the direction (1 clockwise, -1 anticlockwise) for the fastests rotation to a different rotation
	int Rotations::RotDir(float startRot, float targetRot) {

		Rotations::ClampDegreeRotation(targetRot);
		Rotations::ClampDegreeRotation(startRot);

		float angleDiff = targetRot - startRot;

		if (angleDiff > 180)
			angleDiff -= 360;

		if (angleDiff < - 180)
			angleDiff += 360;

		if (angleDiff > 0)
			return 1;

		else
			return -1;
	}