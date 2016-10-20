/*
 * TextureLoader.h
 *
 *  Created on: Nov 21, 2014
 *      Author: dwarf
 */

#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <SDL2/SDL.h>
#include "PNG.h"
#include <string>

namespace warp{

	class Image;

	bool loadTexture(std::string textureName, std::string textureLocation);
	void loadTextureFromBuffer(std::string textureName, int width, char *data);


	struct PreTexture{
		Image *data;
		std::string name;
		unsigned int srcWidth, srcHeight, textureWidth, textureHeight;
	};

	void lockPreTextures();
	void unlockPreTextures();


	class Image{
	public:
		Image();
		Image(PNG *p);
		~Image();
		int getWidth();
		int getHeight();
		int getDepth();
		unsigned char *getPixel();
		void setWidth(int w);
		void setHeight(int h);
		void setDepth(int depth);
		void setPixel(unsigned char *pixel);
	private:
		int width, height, depth;
		unsigned char * pixel;
	};
}

#endif


