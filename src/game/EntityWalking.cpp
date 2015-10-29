/*
 * EntityWalking.cpp
 *
 *  Created on: Dec 21, 2014
 *      Author: dwarf
 */

#include "EntityWalking.h"
#include "World.h"
#include "cmath"
#include "Physics.h"
#include "Game.h"

#include <iostream>

namespace game {

EntityWalking::EntityWalking(float width, float height) : Entity(width, height) {

	canJump = true;
	jumping = false;
	hasReachedGoal = true;
	jumpPowerY = 15;
	jumpPowerX = 10;
	jumpDirection = 0;
	climbing = false;
	climbingX = 0;
	climbingY = 0;
	climbingSpeedX = 3;
	climbingSpeedY = 6;
	goalX = 0;
	goalY = 0;
	movementSpeed = 5;
	walkingDirection = 0;
}

EntityWalking::~EntityWalking() {

}

void EntityWalking::doGameUpdate(float updateFactor){

	int bX = (int)getXCenter();
	int bY = (int)getYCenter();
	if(walkingDirection != 0){
		if(world::isTraversable(bX + walkingDirection, bY)){
			setVelMX(movementSpeed * walkingDirection);
		}
	}
	else{
		setVelMX(0);
	}
	//	if(!hasReachedGoal && !jumping && getVelY() == 0){
	//		//int direction = -(int) ((getXCenter() - goalX) / (abs(getXCenter() - goalX)));
	//		int direction = -getXCenter() + goalX > 0 ? 1 : -1;
	//
	//		//if(World.isCarrying(bX, bY - 1)){
	//		if(world::isTraversable(bX + direction, bY)){
	//			setVelX(movementSpeed * direction);
	//		}
	//		else{
	//			hasReachedGoal = true;
	//		}
	//		//			}
	//		//			else{
	//		//				setVelX(0);
	//		//				if(bY < goalY){
	//		//					hasReachedGoal = true;
	//		//				}
	//		//			}
	//
	//		if(fabs(getXCenter() - goalX) <= 0.2f){
	//			hasReachedGoal = true;
	//			setVelX(0);
	//		}
	//	}
	if(jumping){
		if(getVelX() == 0){
			if((-getXCenter() + goalX) * jumpDirection > 0){
				bX = (int)getXCenter();
				bY = (int)getY();
				if(false && !world::isTraversable(bX + jumpDirection, bY)){ //TODO redo all the climbing stuff
					for(int oY = 1; oY <= (int)getHeight(); oY ++){
						if(!collidesWithWorld(jumpDirection - 0.01f * jumpDirection, oY)){
							climbing = true;
							climbingX = bX + jumpDirection - 0.01f * jumpDirection;
							climbingY = bY + oY;
							climbingSpeedX = 0.05f * jumpDirection;

						}
					}
				}
			}
		}

		if(getVelY() <= 0){
			canJump = true;
		}
		if(canJump && getVelY() == 0){
			setVelX(0);
			jumping = false;
		}
	}

}

int EntityWalking::getWalkingDirection(){
	return walkingDirection;
}

void EntityWalking::jump(){
	if(canJump && !climbing && getVelY() == 0){
		setVelY(jumpPowerY);
//		applyForce(0, jumpPowerY);
	}
}

//void EntityWalking::jumpTo(float x, float y){
//
//	if(canJump && !climbing && getVelY() == 0){
//		goalX = x;
//		int direction = -(int) ((getX() - x) / (fabs(getX() - x)));
//		jumpDirection = direction;
//		float dy = getY() - y;
//		setVelY(jumpPowerY);
//		if(2 * world::getGravity() * dy + jumpPowerY * jumpPowerY >= 0){
//			float t = (float) - ((jumpPowerY + sqrt(2 * world::getGravity() * dy + jumpPowerY * jumpPowerY)) / world::getGravity());
//			float velX = (getX() - x) / t;
//			if(fabs(velX) > jumpPowerX){
//				velX = jumpPowerX * direction;
//			}
//			setVelX(velX);
//		}
//		else{
//			dy = (-(jumpPowerY * jumpPowerY)) / (2 * world::getGravity());
//			if(2 * world::getGravity() * dy + jumpPowerY * jumpPowerY >= 0){
//				float t = (float) - ((jumpPowerY + sqrt(2 * world::getGravity() * dy + jumpPowerY * jumpPowerY)) / world::getGravity());
//				float velX = (getX() - x) / t;
//				if(fabs(velX) > jumpPowerX){
//					velX = jumpPowerX * direction;
//				}
//				setVelX(velX);
//			}
//			else{
//				setVelX(jumpPowerX * direction);
//			}
//		}
//		canJump = false;
//		jumping = true;
//	}
//}

void EntityWalking::handleMovementX(float updateFactor){
	if(!climbing){
		Entity::handleMovementX(updateFactor);
	}
	else{
		if(getY() == climbingY){
			if(fabs(getX() - climbingX) <= fabs(climbingSpeedX * physics::getSimulationSpeed() * updateFactor)){
				setVelX(0);
				climbing = false;
				setX(climbingX);
				jumping = false;
				canJump = true;
			}
			else{
				setX(getX() + climbingSpeedX * physics::getSimulationSpeed() * updateFactor);
			}
		}
	}
}

void EntityWalking::handleMovementY(float updateFactor){
	if(!climbing){
		Entity::handleMovementY(updateFactor);
	}
	else{
		if(getY() != climbingY){
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


//void EntityWalking::walkTo(float x, float y){
//	goalX = x;
//	goalY = y;
//	hasReachedGoal = false;
//}

void EntityWalking::setWalkingDirection(int d){
//	d = d > 0 ? 1 : d == 0 ? 0 : -1;
	walkingDirection = d;
}

void EntityWalking::setJumpPower(float x, float y){
	jumpPowerX = x;
	jumpPowerY = y;
}

void EntityWalking::setJumpPowerY(float jumpPowerY) {
	this->jumpPowerY = jumpPowerY;
}

void EntityWalking::setJumpPowerX(float jumpPowerX) {
	this->jumpPowerX = jumpPowerX;
}

bool EntityWalking::isJumping(){
	return jumping;
}
bool EntityWalking::isHasReachedGoal(){
	return hasReachedGoal;
}
bool EntityWalking::isClimbing(){
	return climbing;
}

void EntityWalking::setMovementSpeed(float s){
	movementSpeed = s;
}

float EntityWalking::getMovementSpeed(){
	return movementSpeed;
}

} /* namespace game */
