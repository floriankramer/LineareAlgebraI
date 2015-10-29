/*
 * NodeAnd.cpp
 *
 *  Created on: Feb 19, 2015
 *      Author: dwarf
 */

#include <NodeAnd.h>
#include "Renderer.h"

namespace game {

render::Color colorTrueAnd(0.5, 1, 0.2);
render::Color colorFalseAnd(1, 0.5, 0.2);

NodeAnd::NodeAnd() {
	setType(3);
	addInput(0, BOOL);
	addInput(1, BOOL);
	addOutput(0, BOOL);
	positive = false;
}

NodeAnd::~NodeAnd() {

}

void NodeAnd::render(bool editMode, float updateFactor){
	if(editMode){
		//		if(getActivation() == getIngoingCount())
		//			render::drawRect(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, colorTrue);
		//		elseif(positive
		if(positive)
			render::drawRect(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, colorTrueAnd);
		else
			render::drawRect(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, colorFalseAnd);
		render::drawString(getXCenter() - 0.5, getYCenter(), 1, "and");
	}
}

void NodeAnd::onInputChanged(int handleId){
	Node::onInputChanged(handleId);
	if(hasHandleData(0) && hasHandleData(1) && getInputHandleData(0).b && getInputHandleData(1).b){
		setHandleData(0, HandleData(true));
		positive = true;
	}
	else{
		setHandleData(0, HandleData(false));
		positive = false;
	}


	//	if(getActivation() == getIngoingCount()){
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
