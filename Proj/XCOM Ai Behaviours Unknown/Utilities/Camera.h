#ifndef CameraH
#define CameraH

#include "../Misc/aimath.h"

	enum ViewType {
		kPerspective, kOrthagraphic
	};

class Camera {

public:
	//constructors and methods
	Camera::Camera(ViewType viewType, float depth, float fov);
	void SetDepthTo(float depth);
	void SetFOVTo(float fov);
	void ModifyPositionBy(Vector3 modifier);
	void ModifyRotationBy(Vector3 angles);
	void MoveGLView();
	void InitView(ViewType type);


	//variables
public:
	Vector3 Position;
	Vector3 Rotation;
	float Angle;

private:
	ViewType _viewType;
	float _fov;
	float _depth;
	Vector3 _lookAt;
	Vector3 _upVector;
};

#endif