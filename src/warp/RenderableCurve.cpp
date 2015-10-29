/*
 * RenderableCurve.cpp
 *
 *  Created on: Mar 6, 2015
 *      Author: dwarf
 */

#include "RenderableCurve.h"
#include "ShaderHandler.h"
#include "MatrixHandler.h"
#include "Warp.h"
#include "math.h"
namespace render {

const int segments = 12;

void renderLine(float x, float y, float x2, float y2, float thickness);

RenderableCurve::RenderableCurve(float x1, float x2, float y1, float y2, float dx1, float dx2, float dy1, float dy2, float thickness, Color c)
 : Renderable(x1, y1, c), x2(x2), y2(y2), thickness(thickness), dx1(dx1), dx2(dx2), dy1(dy1), dy2(dy2){
}

RenderableCurve::~RenderableCurve() {

}

void RenderableCurve::render(){
	shader::setShaderUseTexture(0);
	shader::setShaderColor(color);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(getDefaultVaoId());

	float dX = x2 - x, dY = y2 - y;

	float lastX = x, lastY = y;
	float currentX = 0, currentY = 0;
	float pfac = 0, fac = 0;
	for(int i = 1; i <= segments; i++){
		pfac = (i + 0.0) / segments;
		fac = sin(2 * 3.14 * pfac);
		if(fac > 0){
			currentX = x + dX * pfac + dx1 * fac;// + dx1 * fac + dx2  * (1 - fac);
			currentY = y + dY * pfac + dy1 * fac;// + dy1 * fac + dy2  * (1 - fac);
		}
		else{
			currentX = x + dX * pfac - dx2 * fac;// + dx1 * fac + dx2  * (1 - fac);
			currentY = y + dY * pfac - dy2 * fac;// + dy1 * fac + dy2  * (1 - fac);
		}

		renderLine(lastX, lastY, currentX, currentY, thickness);

		lastX = currentX;
		lastY = currentY;
	}
	glBindVertexArray(0);
}

void renderLine(float x, float y, float x2, float y2, float thickness){
	float length = (float) hypot(x2 - x, y2 - y);

	matrix::loadIdentity();
	matrix::quickTranslate(x + (x2 - x) / 2, y + (y2 - y) / 2);
	matrix::rotate(-atan2(x2 - x, y2 - y));
	matrix::scale((thickness / 2), (length / 2));
	matrix::uploadModelViewMatrix();

	glDrawArrays(GL_TRIANGLES, 0, 6);

}

} /* namespace render */
