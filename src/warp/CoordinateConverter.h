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

float *screenToWorldSpace(float x, float y);

float *worldToScreenSpace(float x, float y);

float *pixelToScreenSpace(int x, int y);

int *screenToPixelSpace(float x, float y);

float *alignWorldToBlockSpace(float x, float y);


#endif /* COORDINATECONVERTER_H_ */
