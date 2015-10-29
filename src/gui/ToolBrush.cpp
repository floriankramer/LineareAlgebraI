/*
 * ToolBrush.cpp
 *
 *  Created on: Feb 6, 2015
 *      Author: dwarf
 */

#include "ToolBrush.h"
#include "World.h"
#include "Mouse.h"
#include "CoordinateConverter.h"

namespace gui {

ToolBrush::ToolBrush() {

}

ToolBrush::~ToolBrush() {

}

void ToolBrush::begin(float x, float y, int button){
	if(button == 1)
		game::world::setBlockAt((int)x, (int)y, 1);
	else if(button == 3)
		game::world::setBlockAt((int)x, (int)y, 0);
}

void ToolBrush::step(float x, float y, int button){
	if(button == 1)
		game::world::setBlockAt((int)x, (int)y, 1);
	else if(button == 3)
		game::world::setBlockAt((int)x, (int)y, 0);
}

void ToolBrush::stop(float x, float y, int button){

}

std::string ToolBrush::getName(){
	return "Brush";
}

void ToolBrush::render(float updateFactor){
	float *f = gui::getMousePosition();
	f = screenToWorldSpace(f[0], f[1]);
	float *w = alignWorldToBlockSpace(f[0], f[1]);
	render::drawRect(w[0], w[1], 1, 1, 0, toolOverlayColor);
	delete[] f;
	delete[] w;
}

} /* namespace gui */
