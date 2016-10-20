/*
 * CoordinateConverter.cpp
 *
 *  Created on: Mar 11, 2015
 *      Author: dwarf
 */


#include "CoordinateConverter.h"
#include "GUIOptions.h"

#include <math.h>

namespace warp{

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
	f[0] = getCameraX() + ( 2 * x / (gui::options::getScreenSpaceWidth()) - 1) * options::getScaledVirtualWidth();
	f[1] = getCameraY() + ((2 - (2 * y / gui::options::getScreenSpaceHeight())) - 1) * options::getScaledVirtualHeight();

	return f;
}

float *worldToScreenSpace(float x, float y){
	float *f = new float[2];
	f[0] = (((x - getCameraX()) / options::getScaledVirtualWidth()) + 1) * (gui::options::getScreenSpaceWidth() / 2);
	f[1] = ( -((y - getCameraY()) / options::getScaledVirtualHeight()) + 1) * (gui::options::getScreenSpaceHeight() / 2);

	return f;
}

float* pixelToScreenSpace(int x, int y){
	float *f = new float[2];
	f[0] = ((x + 0.0) / options::getDisplayHeight()) * gui::options::getScreenSpaceHeight(); // using height works, as we otherwise divide by aspect ratio, and the multiply with it again
	f[1] = ((y + 0.0) / options::getDisplayHeight()) * gui::options::getScreenSpaceHeight();
	return f;
}

int *screenToPixelSpace(float x, float y){
	int *f = new int[2];
	f[0] = static_cast<int>(x / gui::options::getScreenSpaceHeight() * options::getDisplayHeight());
	f[1] = static_cast<int>(y / gui::options::getScreenSpaceHeight() * options::getDisplayHeight());
	return f;
}

float *alignWorldToBlockSpace(float x, float y){
	float *f = new float[2];
	f[0] = floor(x) - (getCameraX() - floor(getCameraX())) + 1;
	f[1] = floor(y) - (getCameraY() - floor(getCameraY())) + 1;
	return f;
}

int *screenToScissorSpace(float x, float y){
	int *f = new int[2];
	f[0] = static_cast<int>(x / gui::options::getScreenSpaceHeight() * options::getDisplayHeight());
	f[1] = static_cast<int>((1 - (y / gui::options::getScreenSpaceHeight())) * options::getDisplayHeight());
	return f;
}

}
