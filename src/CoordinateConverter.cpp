/*
 * CoordinateConverter.cpp
 *
 *  Created on: Mar 11, 2015
 *      Author: dwarf
 */


#include "CoordinateConverter.h"
#include "GUIOptions.h"

#include <math.h>

/**
 * World space has a global origin of 0, 0, relative to which all worlds describe their position.
 * Positive x is right and positive y is up. A size of 1 is the width and height of a single block
 *
 * Screen Space has its origin in the top left corner. Positive x is right and positive y is down.
 * A screen height equals 20 units
 *
 * Pixel Space equals the screens coordinate system. The origin is in the top left corner, a unit of one
 * equals a single pixel
 */

float* screenToWorldSpace(float x, float y){
	float *f = new float[2];
	f[0] = render::getCameraX() + ( 2 * x / (gui::options::getScreenSpaceWidth()) - 1) * render::options::getVirtualWidth();
	f[1] = render::getCameraY() + ((2 - (2 * y / gui::options::getScreenSpaceHeight())) - 1) * render::options::getVirtualHeight();

	return f;
}

float *worldToScreenSpace(float x, float y){
	float *f = new float[2];
	f[0] = (((x - render::getCameraX()) / render::options::getVirtualWidth()) + 1) * (gui::options::getScreenSpaceWidth() / 2);
	f[1] = (((y - render::getCameraY()) / render::options::getVirtualHeight()) + 1) * (gui::options::getScreenSpaceHeight() / 2);

	return f;
}

float* pixelToScreenSpace(int x, int y){
	float *f = new float[2];
	f[0] = ((x + 0.0) / render::options::getDisplayHeight()) * gui::options::getScreenSpaceHeight(); // using height works, as we otherwise divide bi aspect ratio, and the multiply with it again
	f[1] = ((y + 0.0) / render::options::getDisplayHeight()) * gui::options::getScreenSpaceHeight();
	return f;
}

int *screenToPixelSpace(float x, float y){
	int *f = new int[2];
	f[0] = static_cast<int>(x / gui::options::getScreenSpaceHeight() * render::options::getDisplayHeight());
	f[1] = static_cast<int>(y / gui::options::getScreenSpaceHeight() * render::options::getDisplayHeight());
	return f;
}

float *alignWorldToBlockSpace(float x, float y){
	float *f = new float[2];
	f[0] = floor(x) - (render::getCameraX() - floor(render::getCameraX())) + 1;
	f[1] = floor(y) - (render::getCameraY() - floor(render::getCameraY())) + 1;
	return f;
}

int *screenToScissorSpace(float x, float y){
	int *f = new int[2];
	f[0] = static_cast<int>(x / gui::options::getScreenSpaceHeight() * render::options::getDisplayHeight());
	f[1] = static_cast<int>((1 - (y / gui::options::getScreenSpaceHeight())) * render::options::getDisplayHeight());
	return f;
}
