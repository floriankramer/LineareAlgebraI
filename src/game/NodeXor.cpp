/*
 * NodeOr.cpp
 *
 *  Created on: Feb 19, 2015
 *      Author: dwarf
 */

#include <NodeXor.h>
#include "Renderer.h"

namespace game {

render::Color colorTrueXor(0.5, 1, 0.2);
render::Color colorFalseXor(1, 0.5, 0.2);

NodeXor::NodeXor() {
	setType(4);
	addInput(0, BOOL);
	addInput(1, BOOL);
	addOutput(0, BOOL);
}

NodeXor::~NodeXor() {
}

void NodeXor::render(bool editMode, float updateFactor){
	if(editMode){
		if(getOutputHandleData(0).b)
			render::drawRect(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, colorTrueXor);
		else
			render::drawRect(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, colorFalseXor);
		render::drawString(getXCenter() - 0.5, getYCenter(), 1, "Xor");
	}
}

void NodeXor::onInputChanged(int handleId){
	Node::onInputChanged(handleId);

	if(hasHandleData(0) && hasHandleData(1) && getInputHandleData(0).b != getInputHandleData(1).b){
		setHandleData(0, HandleData(true));
	}
	else{
		setHandleData(0, HandleData(false));
	}

	//	if(getActivation() % 2 == 1){
	//		for(Node *n : outgoing){
	//			n->onInputChanged(getUniqueId(), true);
	//		}
	//	}
	//	else{
	//		for(Node *n : outgoing){
	//			n->onInputChanged(getUniqueId(), false);
	//		}
	//	}

}

} /* namespace gui */
