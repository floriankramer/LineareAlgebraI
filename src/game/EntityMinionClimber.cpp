/*
 * EntityMinionClimber.cpp
 *
 *  Created on: Apr 6, 2015
 *      Author: dwarf
 */

#include <EntityMinionClimber.h>

#include "Renderer.h"
#include "World.h"

namespace game {

render::Color colorClimberField(0.7, 0.4, 0.1);

EntityMinionClimber::EntityMinionClimber(): EntityMinion() {
	setJumpPowerY(0);
}

EntityMinionClimber::~EntityMinionClimber() {
}

void EntityMinionClimber::doGameUpdate(float updateFactor){
	EntityMinion::doGameUpdate(updateFactor);
	//	float x = getXCenter();
	//	float y = getYCenter();

	int bX = (int)(getXCenter() + 0.2 * getWalkingDirection());
	int bY = (int)(getYCenter() + 0.2);

	//	if(!world::isTraversable(bX + getWalkingDirection(), bY) || !world::isTraversable(bX + getWalkingDirection(), bY - 1) || (!world::isTraversable(bX + getWalkingDirection(), bY - 2) && world::isTraversable(bX, bY - 2))){
	if(collides(getWalkingDirection() * 0.1, 0)){
		setVelY(2);
	}
	//		for(int oY = 1; oY <= (int)getHeight(); oY ++){
	//			if(!collidesWithLevel(world::getBlocks(), jumpDirection - 0.01f * jumpDirection, oY)){
	//				climbing = true;
	//				climbingX = bX + jumpDirection - 0.01f * jumpDirection;
	//				climbingY = bY + oY;
	//				climbingSpeedX = 0.05f * jumpDirection;
	//
	//			}
	//		}
	//	}

}

void EntityMinionClimber::render(float updateFactor){
	EntityMinion::render(updateFactor);
	render::drawRect(getXCenter(), getYCenter(), 0.5, 0.5, 0, colorClimberField);
}




} /* namespace render */
