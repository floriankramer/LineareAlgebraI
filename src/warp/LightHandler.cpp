/*
 * LightHandler.cpp
 *
 *  Created on: Dec 27, 2014
 *      Author: dwarf
 */

#include "LightHandler.h"
#include <vector>
#include "GraphicsOptions.h"
#include <string>
#include "Renderable.h"
#include "MatrixHandler.h"

namespace render{
namespace light{

const float lightFalloffFactor = 20;

float falloffStrength = 1;
bool playerGlows = true;
float playerGlowStrength = 1;
render::Shader *shader;
std::vector<Light> lights;
bool lightsEnabled = true;

int addLight(float x, float y, float strength){
	Light l;

	l.x = x;
	l.y = y;
	l.strength = strength;
	lights.push_back(l);

	return lights.size() - 1;
}

int getLightNumber(){
	return lights.size();
}

void setLightPosition(int id, float x, float y){
	lights.at(id).x = x;
	lights.at(id).y = y;
}

/**
 *
 * @param p
 * @param offset
 * @return the amount of lights sent to the shader
 */
int uploadLightData(Shader *p, int offset){ //TODO use uniform blocks?

	int s = lights.size() - offset;
	if(s > options::getMaxLights()){
		s = options::getMaxLights();
	}

	for(int i = 0; i < s; i++){
		Light l = lights.at(i + offset);
		if(l.uniformLocPos == -1){
			l.uniformLocPos = glGetUniformLocation(p->getProgramId(), std::string("lights[" + std::to_string(i) + "].position").c_str());
			l.uniformLocStrength = glGetUniformLocation(p->getProgramId(), std::string("lights[" + std::to_string(i) + "].strength").c_str());
		}
		glUniform2f(l.uniformLocPos, l.x, l.y);
		glUniform1f(l.uniformLocStrength, l.strength);
		glUniform1i(glGetUniformLocation(p->getProgramId(), "activeLights"), s);
	}

	return s;

}

void setLightEnabled(bool b){
	lightsEnabled = b;
}

bool isLightsEnabled(){
	return lightsEnabled;
}

Light *getLight(int i){
	return &lights.at(i);
}

void clearLights(){
	lights.clear();
}

void removeLight(int i){
	lights.erase(lights.begin() + i);
}

void render(float updateFactor){
	for(unsigned int i = 0; i < lights.size(); i++){
		Light *l = &lights[i];
		render::drawRect(l->x, l->y, 0.5, 0.5, 0);
		render::drawCircle(l->x, l->y, l->strength * lightFalloffFactor);
	}
}

}
}


