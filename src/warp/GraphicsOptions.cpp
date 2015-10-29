/*
 * GraphicsOptions.cpp
 *
 *  Created on: Nov 22, 2014
 *      Author: dwarf
 */

#include "GraphicsOptions.h"
#include "DisplayHandler.h"
#include "FBOHandler.h"
#include "fstream"
#include "os.h"
#include <cstdint>
#include "Warp.h"

namespace render{

namespace options{

Uint32 pixelFormat;

float virtualHeight = 20;

uint16_t width = 1280;
uint16_t height = 720;
//float aspectRatio = (width + 0.0) / height;
float aspectRatio = 1280.0 / 720;
int maxLights = 8;
bool fullscreen = false;
int fullscreenWidth = 1280, fullscreenHeight = 720;
int fpsCap = 60;
#ifdef unix
int updateTime = 16000000;
#else
#ifdef windows
int updateTime = 16;
#else
int updateTime = 0;
#endif
#endif
float zoom = 1;

Uint32 getPixelFormat(){
	return pixelFormat;
}

void setPixelFormat(Uint32 format){
	pixelFormat = format;
}

float getVirtualHeight(){
	return virtualHeight;
}

float getVirtualWidth(){
	return aspectRatio * virtualHeight;
}

int getDisplayWidth(){
	if(fullscreen)
		return fullscreenWidth;
	else
		return width;
}

int getDisplayHeight(){
	if(fullscreen)
		return fullscreenHeight;
	else
		return height;
}

int getFBOWidth(){
	return width;
}
int getFBOHeight(){
	return height;
}

float getAspectRatio(){
	return aspectRatio;
}

int getMaxLights(){
	return maxLights;
}

void setResolution(int w, int h){
	if(w != width || h != height){
		width = w;
		height = h;
		aspectRatio = (width + 0.0) / height;
		setDisplayResolution(w, h);
		fbo::recreateFBOs = true;
	}
}

bool isFullscreen(){
	return fullscreen;
}

void setFullscreen(bool f){
	fullscreen = f;
}

void setFullscreenResolution(int w, int h){
	fullscreenWidth = w;
	fullscreenHeight = h;
}

int getUpdateTime(){
	return updateTime;
}

void setFPSCap(int cap){
	fpsCap = cap;
	updateTime = 1000000000 / cap;
}

float getPixelWidth(){
	return virtualHeight / height;
}

float getPixelHeight(){
	return getVirtualWidth() / width;
}

void setZoom(float z){
	zoom = z;
}

float getZoom(){
	return zoom;
}

}
}

