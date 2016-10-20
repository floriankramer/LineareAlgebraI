/*
 * RenderableCircle.cpp
 *
 *  Created on: 2016-03-10
 *      Author: dwarf
 */

#include "RenderableCircle.h"
#include "ShaderHandler.h"
#include "MatrixHandler.h"
#include "Warp.h"
namespace warp {

RenderableCircle::RenderableCircle(float x, float y, float radius, Color c) : Renderable(x, y, c), radius(radius) {

}

RenderableCircle::~RenderableCircle() {
}

void RenderableCircle::render(){

	matrix::loadIdentity();
	matrix::quickTranslate(x, y);
	matrix::scale(radius, radius);

	matrix::uploadModelViewMatrix();


	shader::setShaderUseTexture(0);
	shader::setShaderColor(color);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(getCircleVaoId());
	glDrawArrays(GL_TRIANGLE_FAN, 0, 33);
	glBindVertexArray(0);
}

} /* namespace warp */
