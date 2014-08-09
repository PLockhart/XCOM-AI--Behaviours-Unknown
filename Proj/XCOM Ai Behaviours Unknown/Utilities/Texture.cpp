#include "Texture.h"

#include <iostream>
#include "../Utilities/TGALoader.h"

//default constructor
Texture::Texture() { }

//Returns an opengl texture from the passed file, or -1 if not sucessful
TGATexture Texture::loadTexture(char * fileName) {
	
	//create a string from the file name
	std::string temp;
	temp.assign(fileName);

	//load a TGA file if in this format
	//if (Texture::hasEnding(fileName, ".tga"))
		return Texture::loadTGAFile(fileName);
}

//Loads a TGA image and binds it to a texture
TGATexture Texture::loadTGAFile(char * fileName) {

	glEnable(GL_TEXTURE_2D);

	//load the TGA data
	TGATexture texData;
	LoadTGA(&texData, fileName);

	//generate a new texture
	glGenTextures(1, &texData.texID);
	glBindTexture(GL_TEXTURE_2D, texData.texID);

	glTexImage2D(GL_TEXTURE_2D, 0, texData.bpp / 8, texData.width, texData.height, 0, texData.type, GL_UNSIGNED_BYTE, texData.imageData);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	free(texData.imageData);

	return texData;
}

//checks to see if a passed string has the other parameter as the ending
bool Texture::hasEnding (std::string const &fullString, std::string const &ending)
{
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}
