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
#include "glewWrapper.h"
#include "ShaderHandler.h"
#include "CollisionHandler.h"
#include <cmath>
#include "Warp.h"
namespace warp{
namespace light{

const float lightFalloffFactor = 20;

float falloffStrength = 1;
bool playerGlows = false; //obsolete?
float playerGlowStrength = 1;
Shader *shader;
std::vector<Light> lights;
bool lightsEnabled = true;

unsigned int shadowWidth = 256, numShadows = 32;

unsigned int shadowTexture = 0;

bool shouldRebuildShadowMap = false;

void init(){
	char *sb = new char[shadowWidth * 3 * numShadows]; //allocate enough space for 32 lights at 256 pixel resolution with 3 bytes per pixel
	for(unsigned int i = 0; i < shadowWidth * 3 * numShadows; i++){
		sb[i] = 255;
	}
	glGenTextures(1, &shadowTexture);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, shadowWidth, numShadows, 0, GL_RGB, GL_UNSIGNED_BYTE, sb);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	shader::setCurrentShader(shader::getLightShader());
	glUniform1i(glGetUniformLocation(shader::getLightShaderId(), "texture_diffuse"), 0);
	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(shader::getLightShaderId(), "texture_shadow"), 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glActiveTexture(GL_TEXTURE0);
	delete[] sb;
}

void rebuildShadowMap(){
	shouldRebuildShadowMap = true;
}

bool isShouldRebuildShadowMap(){
	return shouldRebuildShadowMap;
}

void rebuildShadowMapInternal(){

/* TODO Florian
 * 	readd independently from world implementation
 *	potentially use list of arbitrary sized rectangles,
 *	and interpolate along their edges
 *
 *
	shouldRebuildShadowMap = false;
//	return;
	warpLogger.log("Rebuilding Shadow map...");

	unsigned char *sb = new unsigned char[shadowWidth * 3 * numShadows];

//	warpLogger.log("allocated memory");

	for(unsigned int i = 0; i < lights.size() && i < numShadows; i++){ //iterate through every light, up to the 32nd
//		warpLogger.log("processing light: " + std::to_string(i));
		Light *l = &lights[i];
		float range = l->strength * lightFalloffFactor;

		for(unsigned int a = 0; a < shadowWidth * 3; a++){
//			warpLogger.log("looking at angle: " + std::to_string(a));

			float angle = (a + 0.0) / shadowWidth / 3 * 6.283185307179586;
			float dX = range * cos(angle);
			float dY = range * sin(angle);

			float minDist = range;

			double minX = dX > 0 ? l->x : l->x + dX;
			double minY = dY > 0 ? l->y : l->y + dY;

			double maxX = dX > 0 ? l->x + dX : l->x;
			double maxY = dY > 0 ? l->y + dY : l->y;



			for(int x = static_cast<int>(minX); x <= static_cast<int>(maxX); x++){ //TODO optimize checked area
				for(int y = static_cast<int>(minY); y <= static_cast<int>(maxY); y++){
					if(!game::world::isTraversable(x, y)){
						float *f = game::physics::getLineRectIntersection(l->x, l->x + dX, l->y, l->y + dY, x, y, x + 1, y + 1);	
						if(!std::isnan(f[0])){
							float r = std::hypot(f[0] - l->x, f[1] - l->y);

							if(r < minDist){

								minDist = r;
							}
						}

						delete[] f;
					}
				}
			}
			sb[i * shadowWidth * 3 + a] = static_cast<int>(minDist / range * 255);
		}

	}

//	warpLogger.log("uploading new shadow map to graphics card");
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, shadowWidth, numShadows, 0, GL_RGB, GL_UNSIGNED_BYTE, sb);
	glBindTexture(GL_TEXTURE_2D, 0);


	warpLogger.log("deleting temporary shadow buffer");
	delete[] sb;

	warpLogger.log("Finished rebuilding shadow map!");
*/
}


void debug(){



	/*glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex2d(0, 0);
	glTexCoord2d(1, 0);
	glVertex2d(1, 0);
	glTexCoord2d(1, 1);
	glVertex2d(1, 1);
	glEnd();*/
}

int getNumShadows(){
	return numShadows;
}

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
			l.uniformLocId = glGetUniformLocation(p->getProgramId(), std::string("lights[" + std::to_string(i) + "].id").c_str());
		}

		glUniform2f(l.uniformLocPos, l.x, l.y);
		glUniform1f(l.uniformLocStrength, l.strength);
		glUniform1i(l.uniformLocId, i + offset);
	}
	glUniform1i(glGetUniformLocation(p->getProgramId(), "activeLights"), s);

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
	if(i != lights.size() - 1){
		for(unsigned int j = 0; j < lights.size(); j++){
			lights[j].uniformLocPos = -1;
		}
	}
	lights.erase(lights.begin() + i);
}

void render(float updateFactor){
	for(unsigned int i = 0; i < lights.size(); i++){
		Light *l = &lights[i];
		drawRect(l->x, l->y, 0.5, 0.5, 0);
		drawCircle(l->x, l->y, l->strength * lightFalloffFactor);
	}
}

}
}


