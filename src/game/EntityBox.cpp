/*
 * EntityBox.cpp
 *
 *  Created on: Aug 31, 2015
 *      Author: dwarf
 */

#include <EntityBox.h>
#include "Renderer.h"
#include "Game.h"

namespace game {

EntityBox::EntityBox() : EntityBox(1, 1){
}

EntityBox::EntityBox(int width, int height) : Entity(width, height){
	setCollider(true);
}

EntityBox::~EntityBox() {
}

void EntityBox::render(float updateFactor){
	render::drawRect(getXCenter(), getYCenter(), getWidth(), getHeight(), 0);
}

} /* namespace game */
