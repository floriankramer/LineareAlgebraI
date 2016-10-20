/*
 * TextureLoader.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: dwarf
 */


#include <SDL2/SDL.h>
//#include <SDL2/SDL_opengl.h>
#include <iostream>
#include <fstream>
#include <png.h>
#include <string>
#include "Mutex.h"
#include "TextureLoader.h"
#include "GraphicsOptions.h"
#include "Warp.h"
#include "PNG.h"
#include "glewWrapper.h"

namespace warp{

char *getFileBuffer(char *filename, int *filesize);
Mutex preTexturesMutex;

bool loadTexture(std::string textureName, std::string textureLocation){
	PNG *png = new PNG(textureLocation);
	png->load();

	PreTexture t;

	Image *i = new Image(png);
	t.data = i;
	t.name = textureName;
	t.srcWidth = png->getWidth();
	t.srcHeight = png->getHeight();
	t.textureWidth = png->getWidth();
	t.textureHeight = png->getHeight();

	addTextureToLoad(t);


	return true;
}

/**
 * @param data:	the data in form of RGBA
 */
void loadTextureFromBuffer(std::string textureName, int width, unsigned char *data){
	PreTexture t;

	Image *p = new Image();
	p->setPixel(data);
	p->setWidth(width);
	p->setHeight(sizeof(data) / 4 / width);

	t.data = p;
	t.name = textureName;
	t.srcWidth = width;
	t.srcHeight = sizeof(data) / 4 / width;
	t.textureWidth = width;
	t.textureHeight = sizeof(data) / 4 / width;

	addTextureToLoad(t);
}

char *getFileBuffer(char *filename, int *filesize){
	using namespace std;
	ifstream in;
	in.open(filename, ios::in | ios::binary);
	if(in.is_open()){
		in.seekg(0, in.end);
		*filesize = in.tellg();
		in.seekg(0, in.beg);
		char *buffer = new char[*filesize];
		in.read(buffer, *filesize);
		return buffer;
	}
	else{
		warpLogger.log("error while opening file:");
		warpLogger.log(filename);
	}
	return NULL;
}

void lockPreTextures(){
	preTexturesMutex.lock();
}

void unlockPreTextures(){
	preTexturesMutex.unlock();
}

Image::Image(): width(0), height(0), depth(0), pixel(NULL){

}

Image::Image(PNG *p): width(p->getWidth()), height(p->getHeight()), depth(p->getDepth()), pixel(NULL){
	unsigned int size = p->getHeight() * p->getWidth() * p->getDepth();
	pixel = new unsigned char[size];
	memcpy(pixel, p->getPixel(), size);
}
Image::~Image(){
	delete[] pixel;
}
int Image::getWidth(){
	return width;
}
int Image::getHeight(){
	return height;
}
int Image::getDepth(){
	return depth;
}
unsigned char *Image::getPixel(){
return pixel;
}
void Image::setWidth(int w){
	width = w;
}
void Image::setHeight(int h){
	height = h;
}
void Image::setDepth(int depth){
	this->depth = depth;
}
void Image::setPixel(unsigned char *pixel){
	this->pixel = pixel;
}


}


