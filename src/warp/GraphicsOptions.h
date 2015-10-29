/*
 * GraphicsOptions.h
 *
 *  Created on: Nov 22, 2014
 *      Author: dwarf
 */


#ifndef GRAPHICSOPTIONS_H
#define GRAPHICSOPTIONS_H

#include <SDL2/SDL.h>
namespace render{

namespace options{

Uint32 getPixelFormat();

float getVirtualHeight();
float getVirtualWidth();
float getAspectRatio();
int getDisplayWidth();
int getDisplayHeight();
int getFBOWidth();
int getFBOHeight();
int getMaxLights();
int getUpdateTime();
float getZoom();

bool isFullscreen();

void setFullscreen(bool f);
void setPixelFormat(Uint32 format);
void setResolution(int, int);
void setFullscreenResolution(int w, int h);
void setFPSCap(int fps);
void setZoom(float z);

float getPixelWidth();
float getPixelHeight();

}
}

#endif

