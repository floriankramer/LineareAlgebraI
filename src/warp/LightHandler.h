/*
 * LightHandler.h
 *
 *  Created on: Dec 27, 2014
 *      Author: dwarf
 */

#ifndef LIGHTHANDLER_H_
#define LIGHTHANDLER_H_

#include "glewWrapper.h"
#include "Shader.h"

namespace render{
namespace light{

struct Light{
	float x, y;
	float strength;

	GLint uniformLocPos = -1;
	GLint uniformLocStrength;
};


int addLight(float x, float y, float strength);
int getLightNumber();
void setLightPosition(int, float, float);
int uploadLightData(Shader *, int);
void setLightEnabled(bool);
bool isLightsEnabled();
Light *getLight(int i);
void clearLights();
void removeLight(int);
void render(float updateFactor);

extern const float lightFalloffFactor;

}
}



#endif /* LIGHTHANDLER_H_ */
