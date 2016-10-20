/*
 * RenderableRect.cpp
 *
 *  Created on: Dec 19, 2014
 *      Author: dwarf
 */

#include "RenderableRect.h"
#include "ShaderHandler.h"
#include "MatrixHandler.h"
#include "Warp.h"
namespace warp {

RenderableRect::RenderableRect(float x, float y, float width, float height, float rot, Color c) {
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->rot = rot;
	this->color = c;
}

RenderableRect::~RenderableRect() {
}

void RenderableRect::render(){

	matrix::loadIdentity();
	matrix::quickTranslate(x, y);
	matrix::rotate(rot);
	matrix::scale((width / 2), (height / 2));

	matrix::uploadModelViewMatrix();


	shader::setShaderUseTexture(0);
	shader::setShaderColor(color);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(getDefaultVaoId());
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

} /* namespace warp */
