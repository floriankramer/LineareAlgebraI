/*
 * ToolLight.cpp
 *
 *  Created on: Feb 6, 2015
 *      Author: dwarf
 */

#include "ToolLight.h"
#include "math.h"
#include <limits>

namespace gui {

ToolLight::ToolLight() {
	current = 0;
	currentIndex = -1;
}

ToolLight::~ToolLight() {
}

void ToolLight::begin(float x, float y, int button){
	if(button == 1){
		currentIndex = render::light::addLight(x, y, 0.01);
		current = render::light::getLight(currentIndex);
	}
	else if(button == 3){
		float min = std::numeric_limits<float>::max(), index = -1;
		render::light::Light *l;
		float dist = 0;
		for(int i = 0; i < render::light::getLightNumber(); i++){
			l = render::light::getLight(i);
			dist = sqrt((x - l->x) * (x - l->x) + (y - l->y) * (y - l->y));
			if(dist < min){
				min = dist;
				index = i;
			}
		}
		if(index != -1){
			render::light::removeLight(index);
		}
	}
}

void ToolLight::step(float x, float y, int button){
	if(current != 0)
		current->strength = sqrt((x - current->x) * (x - current->x) + (y - current->y) * (y - current->y)) / render::light::lightFalloffFactor;

}

void ToolLight::stop(float x, float y, int button){
	if(current != NULL && current->strength == 0){
		render::light::removeLight(currentIndex);
	}

	current = 0;
	currentIndex = -1;
}

std::string ToolLight::getName(){
	return "Light";
}

} /* namespace gui */
