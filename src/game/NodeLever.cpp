/*
 * NodeLever.cpp
 *
 *  Created on: Feb 18, 2015
 *      Author: dwarf
 */

#include <NodeLever.h>
#include "Renderer.h"
#include "Warp.h"
#include "Game.h"

namespace game {

render::Color colorTrueLe(0, 1, 0);
render::Color colorFalseLe(1, 0 ,0);
const int outputId = 0;

NodeLever::NodeLever() {
	state = false;
	setType(1);
	addOutput(outputId, BOOL);
}

NodeLever::~NodeLever() {
}

void NodeLever::activate(ActivationType a, void * data){
	if(a == INTERACTION){
		state = !state;
		if(state){
			setHandleData(outputId, HandleData(true));
		}
		else{
			setHandleData(outputId, HandleData(false));
		}
	}
}

void NodeLever::render(bool editMode, float updateFactor){
	if(state){
		render::drawSprite(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, "leverRight");
		//render::drawRect(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, colorTrueLe);
	}
	else{
		render::drawSprite(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, "leverLeft");
		//render::drawRect(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, colorFalseLe);
	}
}

} /* namespace gui */
