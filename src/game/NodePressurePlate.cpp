/*
 * NodePressurePlate.cpp
 *
 *  Created on: Mar 9, 2015
 *      Author: dwarf
 */

#include "NodePressurePlate.h"
#include "Renderer.h"
#include "Game.h"

namespace game {

NodePressurePlate::NodePressurePlate() : heavy(false){
	setType(6);
	isDown = false;
	lastDownTick = 0;
	addOutput(0, BOOL);
	addInput(0, BOOL);
}

NodePressurePlate::~NodePressurePlate() {
}

void NodePressurePlate::render(bool editMode, float updateFactor){
	if(heavy){
		if(isDown){
			render::drawSprite(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, "pressureplate_down_heavy");
		}
		else{
			render::drawSprite(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, "pressureplate_up_heavy");
		}
	}
	else{
		if(isDown){
			render::drawSprite(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, "pressureplate_down");
		}
		else{
			render::drawSprite(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, "pressureplate_up");
		}
	}
}

void NodePressurePlate::activate(ActivationType a, void * data){
	if(a == COLLISION){
		Entity *e = static_cast<Entity*>(data);
		if(!heavy || (heavy && e->isHeavy())){
			if(!isDown){
				setHandleData(0, HandleData(true));
			}
			isDown = true;
			lastDownTick = getTickCount();
		}
	}
}

void NodePressurePlate::doGameUpdate(float updateFactor){
	if(isDown && lastDownTick != getTickCount()){
		isDown = false;
		setHandleData(0, HandleData(false));
	}
}

void NodePressurePlate::onInputChanged(int handleId){
	Node::onInputChanged(handleId);
	if(handleId == 0){
		heavy = getInputHandleData(0).b;
	}
}

} /* namespace game */
