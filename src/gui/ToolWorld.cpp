/*
 * ToolWorld.cpp
 *
 *  Created on: Jul 4, 2015
 *      Author: dwarf
 */

#include <ToolWorld.h>
#include "World.h"
#include "Mouse.h"
#include "CoordinateConverter.h"
#include <cmath>

namespace gui {

ToolWorld::ToolWorld() : path("./level/lvl1"), width(0), height(0){

}

ToolWorld::~ToolWorld() {
}

void ToolWorld::begin(float x, float y, int button){
	if(button == 1){
		game::world::loadWorld(path, x, y);
	}
	else if(button == 3){
		game::world::unloadWorld(x, y);
	}


}

void ToolWorld::step(float x, float y, int button){
}

void ToolWorld::stop(float x, float y, int button){

}

std::string ToolWorld::getName(){
	return "World";
}

std::string ToolWorld::setParameter(std::string name, std::string value){
	if(name == "path"){
		path = value;
		std::pair<int, int> p = game::world::getWorldSizeFromFile(path);
		width = p.first;
		height = p.second;
		return "set path";
	}
	else{
		return "options: path";
	}
}

void ToolWorld::render(float upateFactor){

	float *f = getMousePosition();
	float *v = screenToWorldSpace(f[0], f[1]);
	float x, y;
	x = floor(v[0]);
	y = floor(v[1]);

	render::drawRect(x + width / 2, y + height / 2, width, height, 0, toolOverlayColor);

	delete[] v;

}

} /* namespace gui */
