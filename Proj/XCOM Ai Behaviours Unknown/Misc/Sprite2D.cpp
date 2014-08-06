#include "../Misc/Sprite2D.h"

//Creates a sprite with a loaded texture
Sprite2D::Sprite2D(TGATexture texture) {

	Width = texture.width;
	Height = texture.height;
	TextureID = texture.texID;
	Rotation = 0;
}

//Draws the texture at its position, from the top left
void Sprite2D::Draw() {

	glPushMatrix();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4f(DisplayColour.r, DisplayColour.g, DisplayColour.b, DisplayColour.a);

	glTranslatef(Position.x, Position.y, 0);
	glRotatef(Rotation, 0, 0, 1);
	glTranslatef(-Width / 2,  -Height / 2, 0);

	glBegin(GL_QUADS);

	glTexCoord2f(0, 1);
	glVertex2f(0, 0);

	glTexCoord2f(1, 1);
	glVertex2f(Width, 0);

	glTexCoord2f(1, 0);
	glVertex2f(Width, Height);

	glTexCoord2f(0, 0);
	glVertex2f(0, Height);

	glEnd();

	glColor4f(1.0, 1.0, 1.0, 1.0);

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}