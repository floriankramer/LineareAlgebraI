/*
 * CoordinateConverter.cpp
 *
 *  Created on: Mar 11, 2015
 *      Author: dwarf
 */


#include "CoordinateConverter.h"
#include <math.h>

float* screenToWorldSpace(float x, float y){
	float *f = new float[2];
	f[0] = render::getCameraX() + x * render::options::getVirtualHeight();
	f[1] = render::getCameraY() + y * render::options::getVirtualHeight();
	return f;
}

float *worldToScreenSpace(float x, float y){
	float *f = new float[2];
	f[0] = (x - render::getCameraX()) / render::options::getVirtualHeight();
	f[1] = (y - render::getCameraY()) / render::options::getVirtualHeight();
	return f;
}

float* pixelToScreenSpace(int x, int y){
	float *f = new float[2];
	f[0] = ((x * 2.0 / render::options::getDisplayWidth()) - 1) * render::options::getAspectRatio();
	f[1] = -((y * 2.0 / render::options::getDisplayHeight()) - 1);
	return f;
}

int *screenToPixelSpace(float x, float y){
	int *f = new int[2];
	f[0] = static_cast<int>(((x / render::options::getAspectRatio()) + 1) * render::options::getDisplayWidth() / 2);
	f[1] = static_cast<int>((-y + 1) / render::options::getDisplayHeight() / 2);
	return f;
}

float *alignWorldToBlockSpace(float x, float y){
	float *f = new float[2];

	f[0] = floor(x) - (render::getCameraX() - floor(render::getCameraX()));
	f[1] = floor(y) - (render::getCameraY() - floor(render::getCameraY())) + 1;

//	f[0] = floor(x);
//	f[1] = floor(y);

	return f;
}
