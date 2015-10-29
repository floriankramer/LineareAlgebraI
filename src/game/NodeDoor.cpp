/*
 * NodeDoor.cpp
 *
 *  Created on: Feb 16, 2015
 *      Author: dwarf
 */

#include "NodeDoor.h"
#include "Renderer.h"
#include <iostream>
#include "Physics.h"
#include <cmath>
#include "AudioPlayer.h"
#include "Game.h"

namespace game {

render::Color colorWood(98 / 255.0, 43 / 255.0, 16 / 255.0);
render::Color colorWoodGray(30 / 255.0, 21 / 255.0, 13 / 255.0);
render::Color colorBlue(0, 0, 1);

const int inputState = 0;
const int inputSpeed = 1;
const int inputStop = 2;
const int inputThump = 3;

NodeDoor::NodeDoor() : soundId(-1) {
	goalY = 0;
	shouldMove = false;
	setCollides(true);
	setMoves(true);
	setType(2);
	setSpeed(5);
	setOpaque(true);

	addInput(inputState, BOOL);
	addInput(inputSpeed, FLOAT);
	addInput(inputStop, BOOL);
	addInput(inputThump, BOOL);
}

NodeDoor::~NodeDoor() {

}

//void NodeDoor::activate(ActivationType a){
//	if(a == NODE && getDirection() == 0){
//		if(getXCenter() == getGoalX() && getYCenter() == getGoalY()){
//			setDirection(-1);
//		}
//		else{
//			setDirection(1);
//		}
//	}
//}

void NodeDoor::onInputChanged(int handleId){
	Node::onInputChanged(handleId);

	if(handleId == inputState || handleId == inputStop){
		if(hasHandleData(inputStop) && getInputHandleData(inputStop).b){
			setDirection(0);

		}
		else{
			if(hasHandleData(inputState) && getInputHandleData(inputState).b){
				if((getSpeedX() == 0 || getXCenter() != getGoalX()) && (getSpeedY() == 0 || getYCenter() != getGoalY())){
					setDirection(1);
				}
			}
			else if((getSpeedX() == 0 || getXCenter() != getInitX()) && (getSpeedY() == 0 || getYCenter() != getInitY())){
				setDirection(-1);
			}
		}
	}
	else if(handleId == inputSpeed){
		if(hasHandleData(inputSpeed)){
			setSpeed(getInputHandleData(inputSpeed).f);
			recalculateMotionData();
		}
	}

	//	if(getActivation() > 0 && (getSpeedX() == 0 || getXCenter() != getGoalX()) && (getSpeedY() == 0 || getYCenter() != getGoalY())){
	//		setDirection(1);
	//	}
	//	else if(getActivation() == 0 && (getSpeedX() == 0 || getXCenter() != getInitX()) && (getSpeedY() == 0 || getYCenter() != getInitY())){
	//		setDirection(-1);
	//	}
}

void NodeDoor::move(float updateFactor){
	Node::move(updateFactor);
	if(soundId == -1 && (this->getMotionX() != 0 || this->getMotionY() != 0)){
		soundId = audio::playSound("stone_slide", getXCenter(), getYCenter(), 1, 1, true);
	}
	else if(soundId != -1 && (this->getMotionX() != 0 || this->getMotionY() != 0)){
		audio::setSoundPosition(soundId, getXCenter(), getYCenter());
	}
	else if(soundId != -1 && this->getMotionX() == 0 && this->getMotionY() == 0){
		audio::stopSound(soundId);
		soundId = -1;
	}
}

void NodeDoor::render(bool editMode, float updateFactor){
	render::drawRect(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, colorWood);
	if(editMode){
		render::drawRect(getGoalX(), getGoalY(), getWidth(), getHeight(), 0, colorWoodGray);
		render::drawLine(getXCenter(), getYCenter(), getGoalX(), getGoalY(), 0.05, colorBlue);
	}
}

} /* namespace gui */
