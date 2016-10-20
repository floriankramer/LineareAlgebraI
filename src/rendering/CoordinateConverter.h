/*
 * CoordinateConverter.h
 *
 *  Created on: Mar 11, 2015
 *      Author: dwarf
 */

#ifndef COORDINATECONVERTER_H_
#define COORDINATECONVERTER_H_

#include "GraphicsOptions.h"
#include "Warp.h"

namespace warp{

	// The user has to free the returned array
	float *screenToWorldSpace(float x, float y);

	// The user has to free the returned array
	float *worldToScreenSpace(float x, float y);

	// The user has to free the returned array
	float *pixelToScreenSpace(int x, int y);

	// The user has to free the returned array
	int *screenToPixelSpace(float x, float y);

	// The user has to free the returned array
	float *alignWorldToBlockSpace(float x, float y);

	// The user has to free the returned array
	int *screenToScissorSpace(float x, float y);

}

#endif /* COORDINATECONVERTER_H_ */
