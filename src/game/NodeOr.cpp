/*
 * NodeOr.cpp
 *
 *  Created on: Apr 27, 2015
 *      Author: dwarf
 */

#include <NodeOr.h>
#include "Renderer.h"

namespace game {

render::Color colorTrueOr(0.5, 1, 0.2);
render::Color colorFalseOr(1, 0.5, 0.2);

NodeOr::NodeOr() : positive(false){
	setType(10);
	addInput(0, BOOL);
	addInput(1, BOOL);
	addOutput(0, BOOL);
}

NodeOr::~NodeOr() {

}



void NodeOr::render(bool editMode, float updateFactor){
	if(editMode){
		if(positive)
			render::drawRect(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, colorTrueOr);
		else
			render::drawRect(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, colorFalseOr);
		render::drawString(getXCenter() - 0.5, getYCenter(), 1, "or");
	}
}

void NodeOr::onInputChanged(int handleId){
	Node::onInputChanged(handleId);
	if(hasHandleData(0) && hasHandleData(1) && getInputHandleData(0).b && getInputHandleData(1).b){
		setHandleData(0, HandleData(true));
		positive = true;
	}
	else{
		setHandleData(0, HandleData(false));
		positive = false;
	}

}

} /* namespace gui */
