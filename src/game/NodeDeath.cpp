/*
 * NodeDeath.cpp
 *
 *  Created on: Feb 19, 2015
 *      Author: dwarf
 */

#include <NodeDeath.h>
#include "Renderer.h"
#include "World.h"
#include "Entity.h"

#include "Game.h"

namespace game {

render::Color color(1, 0.1, 0.05);

NodeDeath::NodeDeath() {
	setType(5);
}

NodeDeath::~NodeDeath() {

}

void NodeDeath::render(bool editMode, float updateFactor){
	render::drawRect(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, color);
}

void NodeDeath::activate(ActivationType a, void * data){
	if(a == COLLISION){
		world::removeEntity(((Entity *)data));
		gameLogger.log("killed entity");
	}
}

} /* namespace gui */
