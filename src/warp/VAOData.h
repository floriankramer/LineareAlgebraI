/*
 * VAOData.h
 *
 *  Created on: Dec 11, 2014
 *      Author: dwarf
 */

#ifndef VAODATA_H_
#define VAODATA_H_

#include "glewWrapper.h"

struct VAOData{
GLuint id;
GLuint vboId;
float minU, minV, maxU, maxV, width, height;
};


#endif /* VAODATA_H_ */
