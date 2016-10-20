/*
 * RenderableStatic.cpp
 *
 *  Created on: 2016-04-01
 *      Author: dwarf
 */

#include "RenderableStatic.h"
#include "ShaderHandler.h"
#include "MatrixHandler.h"
#include "Warp.h"

#include <cmath>


namespace warp {

RenderableStatic::RenderableStatic(StaticRenderable *sr) : Renderable(), sr(sr) {
	
}

RenderableStatic::~RenderableStatic() {

}

void RenderableStatic::render(){

	matrix::loadIdentity();	
	matrix::uploadModelViewMatrix();

	shader::setShaderUseTexture(0);
	shader::setShaderColor(sr->color);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(sr->vaoId);
	glDrawArrays(sr->drawType, 0, sr->vertexCount);
	glBindVertexArray(0);

}

}
