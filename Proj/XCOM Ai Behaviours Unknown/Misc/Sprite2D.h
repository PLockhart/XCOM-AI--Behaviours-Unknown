#ifndef Sprite2DH
#define Sprite2DH

#include "../Utilities/TGALoader.h"
#include "aimath.h"

#include <gl\glut.h>
#include <gl\GL.h>

struct Colour {

	float r, g, b, a;

	Colour() {

		r = g = b = a = 1;
	}

	Colour(float ir, float ig, float ib, float ia) {

		r = ir; g = ig; b = ib; a = ia;
	}
};

class Sprite2D {

	//Variables
	//===================================================
public:
	int Width, Height;	//The width and height of the texture
	Vector3 Position;	//The position to draw the sprite on the screen, centered
	float Rotation;	//The 2d rotation of the sprite, in degrees

	Colour DisplayColour;
	GLuint TextureID;	//The texture identifier

	//Methods
	//===================================================
public:
	//Sprite2D::Sprite2D();
	Sprite2D::Sprite2D(TGATexture texture);
	virtual void Draw();
};

#endif