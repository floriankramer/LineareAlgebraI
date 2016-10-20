/*
 * RenderableScissor.cpp
 *
 *  Created on: Oct 30, 2015
 *      Author: dwarf
 */

#include <RenderableScissor.h>
#include "CoordinateConverter.h"
#include "glewWrapper.h"

namespace warp {

RenderableScissor::RenderableScissor(float x, float y, float width, float height) : Renderable(x, y) {
	this->width = width;
	this->height = height;
}

RenderableScissor::~RenderableScissor() {

}

void RenderableScissor::render(){
	int *f1 = screenToScissorSpace(x, y); //TODO check y orientation returned vs y orientation used by glScissor (up / down)
	int *f2 = screenToPixelSpace(width, height);
	glScissor(f1[0], f1[1], f2[0], f2[1]);

	
	delete[] f1;
	delete[] f2;
}

} /* namespace gui */
