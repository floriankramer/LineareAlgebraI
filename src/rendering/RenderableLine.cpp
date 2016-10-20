/*
 * RenderableLine.cpp
 *
 *  Created on: Dec 19, 2014
 *      Author: dwarf
 */

#include "RenderableLine.h"
#include "ShaderHandler.h"
#include "MatrixHandler.h"
#include "Warp.h"

#include <cmath>


namespace warp {

RenderableLine::RenderableLine(float x, float y, float x2, float y2, float thickness, Color c) {
	this->x = x;
	this->y = y;
	this->x2 = x2;
	this->y2 = y2;
	this->thickness = thickness;
	this->color = c;
}

RenderableLine::~RenderableLine() {

}

void RenderableLine::render(){
	float length = (float) hypot(x2 - x, y2 - y);

	matrix::loadIdentity();
	matrix::quickTranslate(x + (x2 - x) / 2, y + (y2 - y) / 2);
	matrix::rotate(-atan2(x2 - x, y2 - y));
	matrix::scale((thickness / 2), (length / 2));
	matrix::uploadModelViewMatrix();


	shader::setShaderUseTexture(0);
	shader::setShaderColor(color);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(getDefaultVaoId());
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

}

}
