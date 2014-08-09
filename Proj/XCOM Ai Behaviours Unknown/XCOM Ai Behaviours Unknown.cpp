//Peter Lockhart
//40042539

//BUILD AND RUN IN RELEASE MODE FOR SMOOTH PERFORMANCE

//In the main method, modify the level number for different demonstrations
//Be sure to read the top and bottom of the screens during each demonstration for explanations/insights

#include <time.h>
#include <windows.h>
#include <ostream>
#include <iostream>
#include <vector>

#include "Utilities/CharacterDebugger.h"
#include "Utilities/Camera.h"
#include "Level/Level.h"
#include "Utilities/Texture.h"
#include "Characters and Weapons/Character.h"
#include "AI/Actions.h"
#include "Characters and Weapons/Rifle.h"
#include "Characters and Weapons/Shotgun.h"
#include "AI\Team.h"
#include "AI\DecisionTree.h"

#include <gl\glut.h>
#include <gl\GL.h>

//prototypes
void renderScene();
void normalKeysPressed(unsigned char key, int x, int y);
void specialKeysPressed(int key, int x, int y);
void update(int i);

//the level examples
void lobSided1();
void bothSidesDumb();
void cooperativeAssault();
void differentTeamStats();
void evenStats();
void threeWay();

//end level prototyping

	Camera * _gameCamera;
	Level * _level;	//the level that the world exists in
	CharacterDebugger _debugger;

	vector<Team*> _teams;	//the teams that are part of the world
	vector<Character*> _characters;	//the characters in the world

	//fps calculations
	float currentTime = 0;
	float previousTime = 0;
	int targetDt = 25;

int main(int argc, char **argv) {

	char *version = (char*)glGetString(GL_VERSION);

	srand((unsigned)time(0)); 

	//init a window etc
	glutInit(&argc, argv);
	glutInitWindowPosition(-1, -1);
	//make the window fill the screen
	glutInitWindowSize(1240, 768);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("XCOM Ai Behaviours Unknown");

	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	//set up game camera
	_gameCamera = new Camera(kOrthagraphic, 800, 90);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);

	//set up key listeners
	glutKeyboardFunc(normalKeysPressed);
	glutSpecialFunc(specialKeysPressed);

	int levelNum = 0;

	switch (levelNum) {

	case 0:
		lobSided1();
		break;

	case 1:
		cooperativeAssault();
		break;

	case 2:
		differentTeamStats();
		break;

	case 3:
		threeWay();
		break;

	case 4:
		bothSidesDumb();
		break;
	}

	//create debugger hud and add characters to it
	_debugger = CharacterDebugger();
	for (int i = 0; i < (int)_characters.size(); i++) {

		_debugger.addCharacter(_characters[i]);
		_characters[i]->TheDecisionTree->DebugEnabled = true;
	}

	//set up update loop
	glutTimerFunc(1, update, 1);

	//the draw and main loop for opengl
	glutDisplayFunc(renderScene);
	glutMainLoop();
}

void lobSided1() {

	_level = new Level("LobSided1.txt");
	_level->Description = "1 team works together to patrol together to find enemies then suppress and assault them. Other team moves/fights as individuals";
	_level->Footnote = "In this level I wanted to demonstrate how a cooperative team fights together, in comparison to the enemy team that doesn't work together. A cooperative team is one which has team members with comradery. A cooperative rifleman will suppress an enemy unit, reducing their accuracy and movement. This allows for their teammate to safely approach and engage from a strong position and kill it faster. Cooperative allies will also pair off to patrol together";

	_level->loadContent(Texture::loadTexture("Graphics/floorTile.tga"),
	Texture::loadTexture("Graphics/lowCover.tga"),
	Texture::loadTexture("Graphics/HighCover.tga")
	);
	
	Team * team1 = new Team(1);
	Team * team2 = new Team(2);
	_teams.push_back(team1);
	_teams.push_back(team2);

	//register teams
	team1->registerTeams(_teams);
	team2->registerTeams(_teams);

	_characters.push_back(new Character("Graphics/Rifleman1.tga", _level->getSpawnTile(0), new Rifle(), team1));
	_characters.push_back(new Character("Graphics/Shotgunman1.tga", _level->getSpawnTile(1), new Shotgun(), team1));

	_characters.push_back(new Character("Graphics/Rifleman2.tga", _level->getSpawnTile(2), new Rifle(), team2));
	_characters.push_back(new Character("Graphics/Shotgunman2.tga", _level->getSpawnTile(3), new Shotgun(), team2));

	_characters[0]->setBaseComradery(0);
	_characters[1]->setBaseComradery(0);
	_characters[2]->setBaseComradery(8);
	_characters[3]->setBaseComradery(8);
}

void bothSidesDumb() {

	_level = new Level("BothSidesDumb.txt");
	_level->Description = "Both sides don't work together and fight with their own interests in mind";
	_level->Footnote = "This level demonstrates how individuals can belong to a team but fight with their own interests. Each soldier fights as an individual with no conern for their ally";

	_level->loadContent(Texture::loadTexture("Graphics/floorTile.tga"),
	Texture::loadTexture("Graphics/lowCover.tga"),
	Texture::loadTexture("Graphics/HighCover.tga")
	);
	
	Team * team1 = new Team(1);
	Team * team2 = new Team(2);
	_teams.push_back(team1);
	_teams.push_back(team2);

	//register teams
	team1->registerTeams(_teams);
	team2->registerTeams(_teams);

	_characters.push_back(new Character("Graphics/Rifleman1.tga", _level->getSpawnTile(0), new Rifle(), team1));
	_characters.push_back(new Character("Graphics/Shotgunman1.tga", _level->getSpawnTile(1), new Shotgun(), team1));

	_characters.push_back(new Character("Graphics/Rifleman2.tga", _level->getSpawnTile(2), new Rifle(), team2));
	_characters.push_back(new Character("Graphics/Shotgunman2.tga", _level->getSpawnTile(3), new Shotgun(), team2));

	_characters[0]->setBaseComradery(0);
	_characters[1]->setBaseComradery(0);
	_characters[2]->setBaseComradery(0);
	_characters[3]->setBaseComradery(0);
}

void cooperativeAssault() {

	_level = new Level("CooperativeAssault.txt");
	_level->Description = "Cooperative team moves round behind wall to advance on enemy position while unharmed";
	_level->Footnote = "This level demonstrates how when moving to assault a position, the lower team will favour moving along behind the wall to avoid enemy fire while finding a good firing position. The cooperative ally will also suppress enemies that threaten its allies. Because of the riflemans unusually high comradery, they will prioritise the safety of their allies over their desire to attack enemies, even if that mightn't be the best option";

	_level->loadContent(Texture::loadTexture("Graphics/floorTile.tga"),
	Texture::loadTexture("Graphics/lowCover.tga"),
	Texture::loadTexture("Graphics/HighCover.tga")
	);
	
	Team * team1 = new Team(1);
	Team * team2 = new Team(2);
	_teams.push_back(team1);
	_teams.push_back(team2);

	//register teams
	team1->registerTeams(_teams);
	team2->registerTeams(_teams);

	_characters.push_back(new Character("Graphics/Rifleman1.tga", _level->getSpawnTile(0), new Rifle(), team1));
	_characters.push_back(new Character("Graphics/Rifleman1.tga", _level->getSpawnTile(1), new Rifle(), team1));

	_characters.push_back(new Character("Graphics/Rifleman2.tga", _level->getSpawnTile(3), new Rifle(), team2));
	_characters.push_back(new Character("Graphics/Shotgunman2.tga", _level->getSpawnTile(4), new Shotgun(), team2));
	_characters.push_back(new Character("Graphics/Shotgunman2.tga", _level->getSpawnTile(5), new Shotgun(), team2));
	
	int numFirstTeam = 2;

	for (int i = 0; i < numFirstTeam; i++) {

		_characters[i]->setBaseComradery(0);
	}

	for (int i = numFirstTeam; i < (int)_characters.size(); i++) {

		_characters[i]->setBaseAggression(5);
		_characters[i]->setBaseBoldness(4);
		_characters[i]->setBaseComradery(10);
	}
}

void differentTeamStats() {

	_level = new Level("DifferentTeamStats.txt");
	_level->Description = "Left side team has aggressive and bold stats. Right side less aggressive. More aggressive side pushes into other half of identical map";
	_level->Footnote = "The left side has higher boldness and aggressiveness. The extra boldness means they are less concerned about incoming enemy fire and not too worried about taking damage from unprotected sides if they feel like they can inflict good damage. The higher aggression means the left side's characters are less likely to settle for medicore firing positions and will want to go to somewhere better. The right side has low aggression stats and will be more content to fire from their current positions";

	_level->loadContent(Texture::loadTexture("Graphics/floorTile.tga"),
	Texture::loadTexture("Graphics/lowCover.tga"),
	Texture::loadTexture("Graphics/HighCover.tga")
	);
	
	Team * team1 = new Team(1);
	Team * team2 = new Team(2);
	_teams.push_back(team1);
	_teams.push_back(team2);

	//register teams
	team1->registerTeams(_teams);
	team2->registerTeams(_teams);

	_characters.push_back(new Character("Graphics/Shotgunman1.tga", _level->getSpawnTile(0), new Shotgun(), team1));
	_characters.push_back(new Character("Graphics/Rifleman1.tga", _level->getSpawnTile(2), new Rifle(), team1));
	_characters.push_back(new Character("Graphics/Shotgunman1.tga", _level->getSpawnTile(4), new Shotgun(), team1));

	_characters.push_back(new Character("Graphics/Shotgunman2.tga", _level->getSpawnTile(1), new Shotgun(), team2));
	_characters.push_back(new Character("Graphics/Rifleman2.tga", _level->getSpawnTile(3), new Rifle(), team2));
	_characters.push_back(new Character("Graphics/Shotgunman2.tga", _level->getSpawnTile(5), new Shotgun(), team2));
	
	for (int i = 0; i < 3; i++) {

		_characters[i]->setBaseBoldness(10);
		_characters[i]->setBaseComradery(0);
	}

	for (int i = 3; i < (int)_characters.size(); i++) {

		_characters[i]->setBaseComradery(0);
	}
}

void evenStats() {

	_level = new Level("Playground.txt");
	_level->Description = "Both teams identical stats. Mirror play occurs and both sides try to suppress and get into flanking position of other unit";
	_level->Footnote = "This level shows 2 identical teams fighting. Both teams try to flank and outplay the opponent, resulting in mirror play";

	_level->loadContent(Texture::loadTexture("Graphics/floorTile.tga"),
	Texture::loadTexture("Graphics/lowCover.tga"),
	Texture::loadTexture("Graphics/HighCover.tga")
	);

	Team * team1 = new Team(1);
	Team * team2 = new Team(2);
	_teams.push_back(team1);
	_teams.push_back(team2);

	//register teams
	team1->registerTeams(_teams);
	team2->registerTeams(_teams);

	_characters.push_back(new Character("Graphics/Rifleman1.tga", _level->getSpawnTile(0), new Rifle(), team1));
	_characters.push_back(new Character("Graphics/Shotgunman1.tga", _level->getSpawnTile(1), new Shotgun(), team1));

	_characters.push_back(new Character("Graphics/Rifleman2.tga", _level->getSpawnTile(2), new Rifle(), team2));
	_characters.push_back(new Character("Graphics/Shotgunman2.tga", _level->getSpawnTile(3), new Shotgun(), team2));
}

void threeWay() {

	_level = new Level("ThreeWay.txt");
	_level->Description = "3 way identical teams fight";
	_level->Footnote = "This level simply demonstrates how the AI can perform and fight as a group with teams of any size, against any number of teams in the level. Each team has its own awareness, demonstrated by 2 teams fighting it out while the 3rd team is initially unaware of the other enemy team's exsistance or position";

	_level->loadContent(Texture::loadTexture("Graphics/floorTile.tga"),
	Texture::loadTexture("Graphics/lowCover.tga"),
	Texture::loadTexture("Graphics/HighCover.tga")
	);

	Team * team1 = new Team(1);
	Team * team2 = new Team(2);
	Team * team3 = new Team(3);
	_teams.push_back(team1);
	_teams.push_back(team2);
	_teams.push_back(team3);

	//register teams
	team1->registerTeams(_teams);
	team2->registerTeams(_teams);
	team3->registerTeams(_teams);

	_characters.push_back(new Character("Graphics/Shotgunman1.tga", _level->getSpawnTile(0), new Shotgun(), team1));
	_characters.push_back(new Character("Graphics/Rifleman1.tga", _level->getSpawnTile(1), new Rifle(), team1));
	_characters.push_back(new Character("Graphics/Shotgunman1.tga", _level->getSpawnTile(2), new Shotgun(), team1));

	_characters.push_back(new Character("Graphics/Shotgunman2.tga", _level->getSpawnTile(3), new Shotgun(), team2));
	_characters.push_back(new Character("Graphics/Rifleman2.tga", _level->getSpawnTile(5), new Rifle(), team2));
	_characters.push_back(new Character("Graphics/Shotgunman2.tga", _level->getSpawnTile(7), new Shotgun(), team2));

	_characters.push_back(new Character("Graphics/Shotgunman3.tga", _level->getSpawnTile(4), new Shotgun(), team3));
	_characters.push_back(new Character("Graphics/Rifleman3.tga", _level->getSpawnTile(6), new Rifle(), team3));
	_characters.push_back(new Character("Graphics/Shotgunman3.tga", _level->getSpawnTile(8), new Shotgun(), team3));
}

//checking for application exit
void normalKeysPressed(unsigned char key, int x, int y) {

	if (key >= '1' && key <= '5') {

		delete _level;
		_characters.clear();

		while ((int)_teams.size() != 0) {

			delete _teams[0];
			_teams.erase(_teams.begin());
		}
		switch (key) {

		case '1':
			lobSided1();
			break;

		case '2':
			cooperativeAssault();
			break;

		case '3':
			differentTeamStats();
			break;

		case '4':
			threeWay();
			break;

		case '5':
			bothSidesDumb();
			break;
		}

		//create debugger hud and add characters to it
		_debugger = CharacterDebugger();
		for (int i = 0; i < (int)_characters.size(); i++) {

			_debugger.addCharacter(_characters[i]);
			_characters[i]->TheDecisionTree->DebugEnabled = true;
		}

	}

	switch (key) {

			//escape
		case 27:

			exit(0);
			break;
	}

	glutPostRedisplay();
}

//adjusting the frames per second
void specialKeysPressed(int key, int x, int y) {

	float moveSpeed = 0.1f;

	int speedAmount = 8;

	switch (key) {

		case GLUT_KEY_UP:

			targetDt -= speedAmount;
			if (targetDt < speedAmount)
				targetDt = speedAmount;
			break;

		case GLUT_KEY_DOWN:

			targetDt += speedAmount;
			break;
	}

	glutPostRedisplay();
}

void update(int functionNum) {

	//work out the frames per second
	currentTime = glutGet(GLUT_ELAPSED_TIME);

	float dt = currentTime - previousTime;
	//convert to seconds
	dt /= 1000;

	previousTime = currentTime;

	//update all the teams
	for (int i = 0; i < (int)_teams.size(); i++)
		_teams[i]->update(dt);

	//update all the characters
	for (int i = 0; i < (int)_characters.size(); i++)
		_characters[i]->update(dt);

	//reschedule the update and redraw scene at 60fps
	glutTimerFunc(targetDt, update, 1);
	glutPostRedisplay();
}

void renderScene() {

	glDisable(GL_LIGHTING);

	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//reset view
	glLoadIdentity();

	_level->draw();

	//draw all the characters
	for (int i = 0; i < (int)_characters.size(); i++)
		_characters[i]->draw();

	//draw world from a teams perspective
	//_teams[0]->draw();

	_debugger.draw();

	glutSwapBuffers();
}
