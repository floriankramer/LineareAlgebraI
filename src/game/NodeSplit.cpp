/*
 * NodeSplit.cpp
 *
 *  Created on: Mar 11, 2015
 *      Author: dwarf
 */

#include <NodeSplit.h>
#include "Renderer.h"

namespace game {

render::Color colorSplitBackground(0.4, 0.4, 0.4);

NodeSplit::NodeSplit() {
	addInput(0, BOOL);
	addOutput(0, BOOL);
	addOutput(1, BOOL);
	setType(9);
}

NodeSplit::~NodeSplit() {

}

void NodeSplit::render(bool editMode, float updateFactor){
	if(editMode){
		render::drawRect(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, colorSplitBackground);
		render::drawString(getXCenter(), getYCenter(), 1, "split");
	}
}

void NodeSplit::onInputChanged(int handleId){
	if(hasHandleData(handleId)){
		setHandleData(0, getInputHandleData(handleId));
		setHandleData(1, getInputHandleData(handleId));
	}
}

} /* namespace game */
