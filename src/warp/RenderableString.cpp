/*
 * RenderableString.cpp
 *
 *  Created on: Dec 30, 2014
 *      Author: dwarf
 */

#include "RenderableString.h"
#include "glewWrapper.h"
#include "FontRenderer.h"
#include "Warp.h"
#include "MatrixHandler.h"
#include "ShaderHandler.h"
#include "GraphicsOptions.h"


namespace render {

float distance = 0.2;

RenderableString::RenderableString(float x, float y, float height, std::string s, Color c) {
	this->x = x;
	this->y = y;
	this->textHeight = height / 2;
	this->text = s;
	this->color = c;
}

RenderableString::~RenderableString() {

}


void RenderableString::render(){
	Shader * temp = shader::getCurrentShader();
	shader::setCurrentShader(shader::getFontShader());
	shader::setShaderColor(color);
	//	matrix::setupProjectionMatrix(getAspectRatio() * 2, 2, shader::getFontShader());
	matrix::uploadProjectionMatrix();
	glBindTexture(GL_TEXTURE_2D, font::getFontTexture());

	//	shader::setShaderUseTexture(1);
	VAOData *v;
	float i = 0;

	for(char c : text){
		v = font::getVao(c);
		if(v != 0){
			float width = v->width / v->height * textHeight;
			i += width;
			matrix::loadIdentity();
			matrix::quickTranslate(i + x, y - font::getOffset(c) * textHeight);
			matrix::scale(width, -textHeight);

			matrix::uploadModelViewMatrix();

			glBindVertexArray(v->id);

			glDrawArrays(GL_TRIANGLES, 0, 6);
			i += width + distance * textHeight;
		}
		else if(c == ' '){
			i += 3 * distance * textHeight;
		}
		else{
			float width = textHeight;
			i += width;
			matrix::loadIdentity();
			matrix::quickTranslate(i + x, y);
			matrix::scale(width, -textHeight);

			matrix::uploadModelViewMatrix();

			glBindVertexArray(getDefaultVaoId());

			glDrawArrays(GL_TRIANGLES, 0, 6);
			i += width + distance * textHeight;
		}


	}


	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	shader::setCurrentShader(temp);
}


} /* namespace game */
