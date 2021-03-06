#ifndef TextureH
#define TextureH

#include <Windows.h>
#include <string>

#include "../Utilities/TGALoader.h"
#include <gl\glut.h>
#include <gl\GL.h>

class Texture {

public:
	Texture::Texture();
	static TGATexture loadTexture(char * fileName);

private:
	static TGATexture loadTGAFile(char * filename);
	static bool hasEnding (std::string const &fullString, std::string const &ending);
};

#endif