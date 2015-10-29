/*
 * NodeTimer.cpp
 *
 *  Created on: Mar 11, 2015
 *      Author: dwarf
 */

#include <NodeTimer.h>
#include "Renderer.h"

#include "Game.h"

namespace game {

render::Color colorTimerBackground(0.7, 0.7, 0.7);

NodeTimer::NodeTimer() {
	setType(8);
	addInput(0, BOOL);
	addInput(1, FLOAT);
	addOutput(0, BOOL);
	targetState = false;
	delay = 1;
	currentDelay = 0;
	run = false;
}

NodeTimer::~NodeTimer() {
}
void NodeTimer::doGameUpdate(float updateFactor){
	if(run){
		currentDelay -= updateFactor;
		if(currentDelay <= 0){
			run = false;
			currentDelay = delay;
			setHandleData(0, HandleData(targetState));
		}
	}
//	if(isDown && lastDownTick != getTickCount()){
//		isDown = false;
//		setHandleData(0, HandleData(false));
//	}
}

void NodeTimer::render(bool editMode, float updateFactor){
	//if(editMode){
		render::drawRect(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, colorTimerBackground);
		render::drawString(getX() + 0.5, getYCenter(), 1, std::to_string(currentDelay));
	//}
}

void NodeTimer::onInputChanged(int handleId){
	if(handleId == 0){
		if(hasHandleData(0) && getInputHandleData(0).b != targetState){
			targetState = getInputHandleData(0).b;
			currentDelay = delay;
			run = true;
			gameLogger.log("changing state");
		}

	}
	else if(handleId == 1){
		if(hasHandleData(1)){
			delay = getInputHandleData(1).f;
		}
	}
}

} /* namespace game */
