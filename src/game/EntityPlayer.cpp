/*
 * EntityPlayer.cpp
 *
 *  Created on: Dec 23, 2014
 *      Author: dwarf
 */

#include "EntityPlayer.h"
#include <SDL2/SDL_keycode.h>
#include "Keyboard.h"
#include "World.h"
#include "Renderer.h"
#include "Physics.h"
#include "Game.h"
#include "Renderable.h"
#include "AnimationLoader.h"
#include <cmath>

namespace game {

EntityPlayer::EntityPlayer() : Entity(2.1, 4.9) { //1.6, 3.8
	maxSlowDownPower = 500;
	slowDownPower = 500;
	maxMinions = 3;
	jumped = false;
	climbingSpeedX = 3;
	climbingSpeedY = 6;
	climbingX = 0;
	climbingY = 0;
	climbing = false;
	keyJump = false;
	keyMoveLeft = false;
	keyMoveRight = false;
	keyRun = false;
//	renderable = render::createRenderableSprite(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, "player");
//	renderable->setSustain(true);
	character = render::animation::getCharacter("player");
	if(character != NULL)
		character->setAnimation("idle");
}

EntityPlayer::~EntityPlayer() {
//	delete renderable;
	delete character;
}


void EntityPlayer::jump(){
	setVelY(15);
	jumped = false;
}

void EntityPlayer::doGameUpdate(float updateFactor){

}

void EntityPlayer::handleMovementX(float updateFactor){
	if(!climbing){
		Entity::handleMovementX(updateFactor);
	}
	else{
		if(getY() == climbingY){
			if(fabs(getX() - climbingX) <= fabs(climbingSpeedX * physics::getSimulationSpeed() * updateFactor)){
				setVelX(0);
				climbing = false;
				setX(climbingX);
			}
			else{
				setX(getX() + climbingSpeedX * physics::getSimulationSpeed() * updateFactor);
			}
		}
	}
}

void EntityPlayer::onCollideY(int direction){
	if(direction == -1){
		jumped = true;
	}
}

void EntityPlayer::handleMovementY(float updateFactor){
	if(!climbing){
		Entity::handleMovementY(updateFactor);
	}
	else{
		if(getY() != climbingY){ //TODO chenge climbing speed to seconds based system
			if(fabs(getY() - climbingY) <= climbingSpeedY * physics::getSimulationSpeed() * updateFactor){
				setVelY(0);
				setY(climbingY);
			}
			else{
				setY(getY() + climbingSpeedY * physics::getSimulationSpeed() * updateFactor);
			}
		}
	}
}

render::Color overlay(0.7, 0.7, 0.7, 0.5);

void EntityPlayer::render(float updateFactor){
	//	render::drawRect(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, overlay);
	if(character == NULL){
		character = render::animation::getCharacter("player");
		if(character != NULL)
			character->setAnimation("idle");
		else
			render::drawRect(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, overlay);
	}
	else{
		character->updateAnimation(updateFactor);
		character->render(getXCenter(), getYCenter(), getFacing());
	}



	//		renderable->setTransform(getXCenter(), getYCenter(), getWidth(), getHeight());
	//		render::drawRenderable(renderable);
	//render::drawSprite(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, "player");
}

int EntityPlayer::getSlowDownPower() {
	return slowDownPower;
}

int EntityPlayer::getMaxSlowDownPower() {
	return maxSlowDownPower;
}

int EntityPlayer::getMaxMinions() {
	return maxMinions;
}

bool EntityPlayer::hasJumped(){
	return jumped;
}

void EntityPlayer::setKeyJump(bool b){
	keyJump = b;
}

void EntityPlayer::setKeyRun(bool b){
	keyRun = b;
}

void EntityPlayer::setKeyMoveRight(bool b){
	keyMoveRight = b;
	if(b){
		character->setAnimation("walk");
	}
	else if(!getKeyMoveLeft()){
		character->setAnimation("idle");
	}
}

void EntityPlayer::setKeyMoveLeft(bool b){
	keyMoveLeft = b;
	if(b){
		character->setAnimation("walk");
	}
	else if(!getKeyMoveRight()){
		character->setAnimation("idle");
	}
}

bool EntityPlayer::getKeyJump(){
	return keyJump;
}
bool EntityPlayer::getKeyRun(){
	return keyRun;
}
bool EntityPlayer::getKeyMoveRight(){
	return keyMoveRight;
}
bool EntityPlayer::getKeyMoveLeft(){
	return keyMoveLeft;
}

} /* namespace game */
