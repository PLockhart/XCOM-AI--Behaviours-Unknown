#include "CharacterDebugger.h"

#include <sstream>

#include "../Characters and Weapons/BaseWeapon.h"
#include "../Misc/aimath.h"
#include <gl\glut.h>
#include <gl\GL.h>

void CharacterDebugger::AddCharacter(Character * newChar) {

	_characters.push_back(newChar);
}

//Draws all of the character's information
void CharacterDebugger::Draw() {

	Vector3 drawPos = Vector3(1000, 50, 0);

	for (int i = 0; i < (int)_characters.size(); i++) {

		Character * loopedCharacter = _characters[i];
		if (loopedCharacter->IsDead == true)
			continue;

		glPushMatrix();

		//draw the character summary
		glRasterPos2f(drawPos.x, drawPos.y);

		if (loopedCharacter->IsDead == false) {

			if (loopedCharacter->HasAction == true)
				DrawString(loopedCharacter->ToString() + ": " + loopedCharacter ->CurrentAct->ToString());
			else
				DrawString(loopedCharacter->ToString() + ": Idle");
		}
		else
			DrawString(loopedCharacter->ToString() + ": KIA");

		drawPos.y += 20;
		//draw their health bar
		float perHp = loopedCharacter->Health / (float)loopedCharacter->MaxHealth;
		int length = 100;
		int greenLength = (perHp * length);
		int height = 10;

		//the hp remaining
		glColor4f(0, 1, 0, 1);
		glBegin(GL_QUADS);
		glVertex2f(drawPos.x, drawPos.y);
		glVertex2f(drawPos.x + greenLength, drawPos.y);
		glVertex2f(drawPos.x + greenLength, drawPos.y + height);
		glVertex2f(drawPos.x, drawPos.y + height);
		glEnd();
		glColor4f(1, 1, 1, 1);

		
		//the hp gone
		glColor4f(1, 0, 0, 1);
		glBegin(GL_QUADS);
		glVertex2f(drawPos.x + greenLength, drawPos.y);
		glVertex2f(drawPos.x + length, drawPos.y);
		glVertex2f(drawPos.x + length, drawPos.y + height);
		glVertex2f(drawPos.x + greenLength, drawPos.y + height);
		glEnd();
		glColor4f(1, 1, 1, 1);
		
		drawPos.y += 10;

		//the ammo remaining
		float ammoRation = loopedCharacter->Weapon->GetAmmoRatio();
		int yellowLength = (ammoRation * length);

		glColor4f(1, 1, 0, 1);
		glBegin(GL_QUADS);
		glVertex2f(drawPos.x, drawPos.y);
		glVertex2f(drawPos.x + yellowLength, drawPos.y);
		glVertex2f(drawPos.x + yellowLength, drawPos.y + height);
		glVertex2f(drawPos.x, drawPos.y + height);
		glEnd();
		glColor4f(1, 1, 1, 1);

		
		//the ammo gone
		glColor4f(0, 0, 1, 1);
		glBegin(GL_QUADS);
		glVertex2f(drawPos.x + yellowLength, drawPos.y);
		glVertex2f(drawPos.x + length, drawPos.y);
		glVertex2f(drawPos.x + length, drawPos.y + height);
		glVertex2f(drawPos.x + yellowLength, drawPos.y + height);
		glEnd();
		glColor4f(1, 1, 1, 1);

		drawPos.y += 20;

		//draw a potrait of the character
		int portraitHeight = 50;
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, loopedCharacter->TextureID);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(drawPos.x, drawPos.y);
		glTexCoord2f(1, 0);
		glVertex2f(drawPos.x + portraitHeight, drawPos.y);
		glTexCoord2f(1, 1);
		glVertex2f(drawPos.x + portraitHeight, drawPos.y + portraitHeight);
		glTexCoord2f(0, 1);
		glVertex2f(drawPos.x, drawPos.y + portraitHeight);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		drawPos.y += 10;

		//draw some of the starts
		glRasterPos2f(drawPos.x + portraitHeight + 10, drawPos.y);
		DrawString(CombineStrAndNum("Aggression ", loopedCharacter->Aggression));
		drawPos.y += 25;

		glRasterPos2f(drawPos.x + portraitHeight + 10, drawPos.y);
		DrawString(CombineStrAndNum("Boldness ", loopedCharacter->Boldness));
		drawPos.y += 25;

		glRasterPos2f(drawPos.x + portraitHeight + 10, drawPos.y);
		DrawString(CombineStrAndNum("Comradery ", loopedCharacter->Comradery));
		drawPos.y += 25;

		drawPos.y += 50;
		
		glPopMatrix();
	}
}

//Draws a string on the screen
void CharacterDebugger::DrawString(string input) {

	for (int i = 0; i < (int)input.size(); i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, input[i]);
}

string CharacterDebugger::CombineStrAndNum(string input, int num) {

	std::stringstream output;
	output << input;
	output << num;

	return output.str();
}