/*
 * ShaderHandler.cpp
 *
 *  Created on: Dec 19, 2014
 *      Author: dwarf
 */

#include "ShaderHandler.h"
#include "glewWrapper.h"
#include "GraphicsOptions.h"
#include "MatrixHandler.h"
#include "os.h"
#include "Warp.h"

namespace render{
namespace shader{

Shader *main, *postProcessing, *light, *font;
GLint mainUseTextureLocation, mainColorLocation, cameraXLocation, cameraYLocation;;
Shader *current;

void init(){
	main = new Shader(std::string("./shader/main.shader"));
	mainUseTextureLocation = glGetUniformLocation(main->getProgramId(), "useTexture");
	mainColorLocation = glGetUniformLocation(main->getProgramId(), "color");
	cameraXLocation =  glGetUniformLocation(main->getProgramId(), "cameraX");
	cameraYLocation =  glGetUniformLocation(main->getProgramId(), "cameraY");

	setCurrentShader(main);
	glUniform1i(mainUseTextureLocation, 1);
	glUniform4f(mainColorLocation, 1, 1, 1, 1);
	glUseProgram(0);

	checkForGLError("post main shader init");

	postProcessing = new Shader(std::string("./shader/postProcessing.shader"));

	font = new Shader(std::string("./shader/font.shader"));
	setCurrentShader(font);
	matrix::setupProjectionMatrix(options::getAspectRatio() * 2, 2);
	glUseProgram(0);

	light = new Shader(std::string("./shader/light.shader"));
}



void destroy(){
	delete main;
	delete postProcessing;
	delete light;
	delete font;
}

Shader* getMainShader(){
	return main;
}

int getMainShaderId(){
	return main->getProgramId();
}

void setShaderUseTexture(bool b){
	glUniform1i(mainUseTextureLocation, b);
}
void setShaderColor(Color c){
	glUniform4f(mainColorLocation, c.r, c.g, c.b, c.a);
}

void uploadCameraLocation(float x, float y){
	glUniform1f(cameraXLocation, x);
	glUniform1f(cameraYLocation, y);
}

Shader* getPostProcessingShader(){
	return postProcessing;
}
int getPostProcessingShaderId(){
	return postProcessing->getProgramId();
}

Shader* getLightShader(){
	return light;
}
int getLightShaderId(){
	return light->getProgramId();
}

Shader* getFontShader(){
	return font;
}
int getFontShaderId(){
	return font->getProgramId();
}

Shader *getCurrentShader(){
	return current;
}
void setCurrentShader(Shader *s){
	current = s;
	glUseProgram(s->getProgramId());
}

}
}
