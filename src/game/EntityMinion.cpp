/*
 * EntityMinion.cpp
 *
 *  Created on: Dec 21, 2014
 *      Author: dwarf
 */

#include "EntityMinion.h"
#include "../warp/Renderer.h"
#include <string>
#include "World.h"
#include <cmath>
#include "CollisionHandler.h"
#include "Game.h"
#include "LightHandler.h"
#include "AudioPlayer.h"


namespace game {


unsigned int commandDepth = 3;
int getCommandDepth(){
	return commandDepth;
}

void setCommandDepth(int i){
	commandDepth = i;
	for(EntityMinion *m : *game::world::getMinions()){
		m->onCommandDepthChanged(commandDepth);
	}
}

render::Color colorSelectHighlight;

render::Color colorIdle(0, 1, 0);
render::Color colorWorking(1, 0, 0);
std::string commandIcons[] = {"minionMoveR", "minionMoveL", "minionJump", "minionActivate", "minionStop"};

int minionCommand = 0;

EntityMinion* selectedMinion = NULL;
int currentCommand = 0;
int minionCount = 0;
int delCount = 0;

EntityMinion::EntityMinion() : EntityWalking(2, 2), hasSpecial(false), commandPos(0), heavy(false) {
	setJumpPowerX(0.2f);
	//	commands.push_back("move");
	//	commands.push_back("jump");
	for(unsigned int i = 0; i < commandDepth ; i++){
//		commands[i] = -1;
		commands.push_back(-1);
	}
}

EntityMinion::~EntityMinion() {
}

void EntityMinion::onCommandDepthChanged(unsigned int newCommandDepth){
	if(newCommandDepth > commands.size()){
		int d = newCommandDepth - commands.size();
		for(int i = 0; i < d; i++){
			commands.push_back(-1);
		}
	}
}

void EntityMinion::renderBasics(float updateFactor){
	if(this == selectedMinion)
			render::drawRectOutline(getXCenter(), getYCenter(), getWidth() + 0.12f, getHeight() + 0.12f, 0, 0.06f, colorSelectHighlight);
		if(!isHasReachedGoal() || isJumping() || isClimbing()){
			render::drawRect(getXCenter(), getYCenter() + getWidth(), 0.1f, 0.1f, 0, colorWorking);
		}
		else{
			render::drawRect(getXCenter(), getYCenter() + getWidth(), 0.1f, 0.1f, 0, colorIdle);
		}

		if(canSeeLight()){
			render::drawRect(getXCenter(), getYCenter() + getWidth() * .5, 0.5f, 0.5f, 0, colorIdle);
			float *f = getSeenLight();
			render::drawLine(getXCenter(), getYCenter(), f[0], f[1], 0.1);
			delete[] f;
		}

		render::drawRectOutline(getX() + 0.85 * commandPos, getY() + getHeight(), 0.75, 0.75, 0, 0.05);

		for(int i = 0; i < getCommandDepth(); i++){
			if(commands[i] >= 0 && commands[i] < 5){
				render::drawSprite(getX() + 0.85 * i, getY() + getHeight(), 0.75, 0.75, 0, commandIcons[commands[i]]);
			}
			//		switch(commands[i]){
			//		case 0:
			//
			//			break;
			//		case 1:
			//			render::drawSprite(getX() + 0.85 * i, getY() + getHeight(), 0.75, 0.75, 0, "minionMoveL");
			//			break;
			//		case 2:
			//			render::drawSprite(getX() + 0.85 * i, getY() + getHeight(), 0.75, 0.75, 0, "minionJump");
			//			break;
			//		case 3:
			//			render::drawSprite(getX() + 0.85 * i, getY() + getHeight(), 0.75, 0.75, 0, "minionActivate");
			//			break;
			//		case 4:
			//			render::drawSprite(getX() + 0.85 * i, getY() + getHeight(), 0.75, 0.75, 0, "minionActivate");
			//			break;
			//		}
		}
}

void  EntityMinion::render(float updateFactor){
	render::drawSprite(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, "minion_default");
	renderBasics(updateFactor);

}

void EntityMinion::onSpawn(){
	minionCount ++;
	world::addToMinions(this);
}

void EntityMinion::onDeath(){
	audio::playSound("death", getXCenter(), getYCenter(), 1.0, 1.0);
	EntityWalking::onDeath();
	minionCount --;
	world::removeFromMinions(this);
}

void EntityMinion::doNextCommand(){
	if(canSeeLight()){
		switch(commands[0]){
		case 0:
			//		walkTo(getX() + 255, getY());
			setWalkingDirection(1);
			break;
		case 1:
			setWalkingDirection(-1);
			//		walkTo(getX() - 255, getY());
			break;
		case 2:
			jump();
			//jumpTo(getX(), getY() + 5);
			break;
		case 3:
			for(int i = 0; i < world::getNodeCount(); i++){
				Node *n = world::getNode(i);
				if(physics::pointInRect(getX(), getY(), n->getX() - 0.5, n->getY() - 0.5, n->getWidth() + 0.5, n->getHeight() + 0.5)){
					n->activate(INTERACTION, this);
				}
			}
			break;
		case 4:
			setWalkingDirection(0);
			break;
		}

		for(int i = 0; i < getCommandDepth() - 1; i++){
			commands[i] = commands[i + 1];
		}
		commands[getCommandDepth() - 1] = -1;
		if(commandPos > 0)
			commandPos --;
	}
}



//void EntityMinion::decodeCommand(std::string answer){
//	if(answer.length() == 1){
//		int i = std::stoi(answer);
//		if(i >=0 && i < commandDepth)
//			commands[i] = -1;
//	}
//
//	int slot = -1;
//	if(answer[0] == '1'){
//		slot = 0;
//	}
//	else if(answer[0] == '2'){
//		slot = 1;
//	}
//	else if(answer[0] == '3'){
//		slot = 2;
//	}
//	if(slot == -1)
//		return;
//	std::string com = answer.substr(2, answer.length());
//	if(com == "move right"){
//		commands[slot] = 0;
//	}
//	else if(com == "move left"){
//		commands[slot] = 1;
//	}
//	else if(com == "jump"){
//		commands[slot] = 2;
//	}
//	else if(com == "activate"){
//		commands[slot] = 3;
//	}
//
//}

bool EntityMinion::getHasSpecial(){
	return hasSpecial;
}

void EntityMinion::setHasSpecial(bool hasSpecial){
	this->hasSpecial = hasSpecial;
}

int EntityMinion::getCommand(int i){
	return commands[i];
}

void EntityMinion::setCommand(int i, int c){
	if(i >= 0 && i < getCommandDepth()){
		commands[i] = c;
	}

}

void EntityMinion::appendCommand(int c){
	commands[commandPos] = c;
	commandPos ++;
	commandPos %= getCommandDepth();
}

int getMinionCount() {
	return minionCount;
}

void setSelectedMinion(EntityMinion *e){
	selectedMinion = e;
}

EntityMinion* getSelectedMinion(){
	return selectedMinion;
}

float* EntityMinion::getSeenLight(){
	double range;
	float *f = new float[4];
	for(int i = 0 ; i < render::light::getLightNumber(); i++){
		render::light::Light *l = render::light::getLight(i);
		range = l->strength * render::light::lightFalloffFactor;
		if(hypot(l->x - getXCenter(), l->y - getYCenter()) < range){
			double minX = getXCenter() < l->x ? getXCenter() : l->x;
			double minY = getYCenter() < l->y ? getYCenter() : l->y;
			double maxX = getXCenter() < l->x ? l->x : getXCenter();
			double maxY = getYCenter() < l->y ? l->y : getYCenter();

			bool sees = true;
			for(int x = static_cast<int>(minX); x <= static_cast<int>(maxX); x++){
				for(int y = static_cast<int>(minY); y <= static_cast<int>(maxY); y++){
					if(!game::world::isTraversable(x, y) && game::physics::lineIntersectsRect(getXCenter(), l->x, getYCenter(), l->y, x, y, x + 1, y + 1)){
						sees = false;
						y = static_cast<int>(maxY + 1);
						x = static_cast<int>(maxX + 1);
					}
				}
			}
			if(sees){
				for(int j = 0; j < world::getNodeCount() && sees; j++){
					Node *n = world::getNode(j);
					if(n->isOpaque() &&  game::physics::lineIntersectsRect(getXCenter(), l->x, getYCenter(), l->y, n->getX(), n->getY(), n->getX() + n->getWidth(), n->getY() + n->getHeight())){
						sees = false;
					}
				}
			}
			if(sees){
				f[0] = l->x;
				f[1] = l->y;
				f[2] = maxX;
				f[3] = maxY;
				return f;
			}
		}
	}
	return NULL;
}


bool EntityMinion::canSeeLight(){
	double range;
	for(int i = 0 ; i < render::light::getLightNumber(); i++){
		render::light::Light *l = render::light::getLight(i);
		range = l->strength * render::light::lightFalloffFactor;
		if(hypot(l->x - getXCenter(), l->y - getYCenter()) < range){
			double minX = getXCenter() < l->x ? getXCenter() : l->x;
			double minY = getYCenter() < l->y ? getYCenter() : l->y;
			double maxX = getXCenter() < l->x ? l->x : getXCenter();
			double maxY = getYCenter() < l->y ? l->y : getYCenter();

			bool sees = true;
			for(int x = static_cast<int>(minX); x <= static_cast<int>(maxX); x++){
				for(int y = static_cast<int>(minY); y <= static_cast<int>(maxY); y++){
					if(!game::world::isTraversable(x, y) && game::physics::lineIntersectsRect(getXCenter(), l->x, getYCenter(), l->y, x, y, x + 1, y + 1)){
						sees = false;
						y = static_cast<int>(maxY + 1);
						x = static_cast<int>(maxX + 1);
					}
				}
			}
			if(sees){
				for(int j = 0; j < world::getNodeCount() && sees; j++){
					Node *n = world::getNode(j);
					if(n->isOpaque() && game::physics::lineIntersectsRect(getXCenter(), l->x, getYCenter(), l->y, n->getX(), n->getY(), n->getX() + n->getWidth(), n->getY() + n->getHeight())){
						sees = false;
					}
				}
			}

			if(sees){
				return true;
			}
		}
	}
	return false;
}




} /* namespace game */
