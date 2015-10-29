/*
 * EntityMinionHeavy.cpp
 *
 *  Created on: Aug 28, 2015
 *      Author: dwarf
 */

#include <EntityMinionHeavy.h>
#include "Renderer.h"

namespace game {

EntityMinionHeavy::EntityMinionHeavy() : EntityMinion() {
	setJumpPowerY(0);
	setMovementSpeed(3);
	setHeavy(true);
	setPusher(true);
}

EntityMinionHeavy::~EntityMinionHeavy() {

}

void EntityMinionHeavy::render(float updateFactor){
	render::drawSprite(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, "minion_heavy");
	renderBasics(updateFactor);
}


void EntityMinionHeavy::doGameUpdate(float updateFactor){
	EntityMinion::doGameUpdate(updateFactor);

}

} /* namespace gui */
