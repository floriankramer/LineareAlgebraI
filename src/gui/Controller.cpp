/*
 * Controller.cpp
 *
 *  Created on: Apr 15, 2015
 *      Author: dwarf
 */

#include <Controller.h>
#include "SDL2/SDL_keycode.h"
#include "SDL2/SDL_mouse.h"
#include "Vec2f.h"
#include "Widget.h"
#include "Game.h"
#include "Warp.h"
#include "World.h"
#include "Mouse.h"
#include "Physics.h"
#include "CoordinateConverter.h"
#include <cmath>
#include "AudioPlayer.h"
#include "WidgetMinionCommandSelector.h"
#include "Renderer.h"

#include "EntityMinion.h"
#include "EntityMinionClimber.h"
#include "EntityMinionHeavy.h"

namespace gui {

const float cameraSpeed = 1;

float *spawnPos = 0;
bool spawningMinion = false;

WidgetMinionCommandSelector commandSelector;

bool canPlayerReach(float x, float y){
	bool b = hypot(game::getPlayer()->getXCenter() - x, game::getPlayer()->getYCenter() - y) < 5;
	if(!b){
		audio::playPlainSound("out_of_reach");
	}

	return b;
}

void spawnMinion(game::EntityMinion *min, float x, float y){
	if(canPlayerReach(x, y)){
		min->setPositionCenter(x, y);
		//game::world::spawnEntity(min);
		if(min->canSpawn()){
			game::world::spawnEntity(min);
		}

	}
}

game::EntityMinion *getClosest(){
	game::EntityMinion *closest = NULL;
	float closestDis = 9999;
	float dist = 0;
	for(unsigned int i = 0; i < game::world::getMinions()->size(); i++){
		dist = hypot(game::getPlayer()->getXCenter() - game::world::getMinions()->at(i)->getXCenter(), game::getPlayer()->getYCenter() - game::world::getMinions()->at(i)->getYCenter());
		if(dist < 5 && dist < closestDis){
			closest = game::world::getMinions()->at(i);
			closestDis = dist;
		}
	}
	return closest;
}

Controller::Controller() : player(NULL), mouseX(0), mouseY(0), inputType(1), commandFree(1), altCommands(0), selectedIndex(-1){
	axisValues[0] = 0;
	axisValues[1] = 0;
	axisValues[2] = 0;
	axisValues[3] = 0;
}

Controller::~Controller() {

}

void Controller::render(float updateFactor){
	if(inputType == 1){
		render::drawRect(mouseX, mouseY, .05, .05, 3.1415 / 4);
	}
}

void Controller::handleKey(int key, bool pressed){
	if(key == SDLK_p && pressed){
		inputType ++;
		inputType %= 2;
	}

	if(!getConsoleActive() && inputType == 0){
		switch(key){
		case SDLK_a:
			if(!getEditMode() && !game::physics::isPaused()){
				game::getPlayer()->setKeyMoveLeft(pressed);
			}
			else{
				game::setScrollVelocityX(-cameraSpeed * pressed);
			}
			break;
		case SDLK_d:
			if(!getEditMode() && !game::physics::isPaused()){
				game::getPlayer()->setKeyMoveRight(pressed);
			}
			else{
				game::setScrollVelocityX(cameraSpeed * pressed);
			}
			break;
		case SDLK_w:
			if(!getEditMode() && !game::physics::isPaused()){
				game::getPlayer()->setKeyJump(pressed);
			}
			else{
				game::setScrollVelocityY(cameraSpeed * pressed);
			}
			break;
		case SDLK_s:
			if(getEditMode()){
				game::setScrollVelocityY(-cameraSpeed * pressed);
			}
			break;
		case SDLK_c:
			if(!getEditMode() && !game::physics::isPaused()){
				if(game::getMinionCount() < 3){
					if(pressed){
						if(!spawningMinion){
							spawningMinion = true;
							spawnPos = gui::getMousePosition();
							game::getWidgetWorld()->displayMinionSpawnMenu(spawnPos[0], spawnPos[1]);
							spawnPos = screenToWorldSpace(spawnPos[0], spawnPos[1]);
						}
					}
					else{
						spawningMinion = false;
						std::string s = game::getWidgetWorld()->removeMinionSpawnMenu();
						game::EntityMinion *m = NULL;
						if(s == "default"){
							m = new game::EntityMinion();
						}
						else if(s == "climber"){
							m = new game::EntityMinionClimber();
						}
						if(m != NULL && spawnPos != NULL && game::world::withinWorld(spawnPos[0], spawnPos[1])){
							spawnMinion(m, spawnPos[0], spawnPos[1]);
							delete[] spawnPos;
						}

					}
				}
			}
			break;
		case SDLK_1:
			if(pressed && !getEditMode() && !game::physics::isPaused()){
				if(game::getMinionCount() < 3){
					//					int x, y;
					//					SDL_GetMouseState(&x, &y);

					float *f = gui::getMousePosition();

					float *v = screenToWorldSpace(f[0], f[1]);

					spawnMinion(new game::EntityMinion(), v[0], v[1]);
					delete[] v;
				}
			}
			break;
		case SDLK_2:
			if(pressed && !getEditMode() && !game::physics::isPaused()){
				if(game::getMinionCount() < 3){
					float *f = gui::getMousePosition();

					float *v = screenToWorldSpace(f[0], f[1]);

					spawnMinion(new game::EntityMinionClimber(), v[0], v[1]);

					delete[] v;
				}
			}
			break;
		case SDLK_3:
			if(pressed && !getEditMode() && !game::physics::isPaused()){
				if(game::getMinionCount() < 3){
					float *f = gui::getMousePosition();

					float *v = screenToWorldSpace(f[0], f[1]);

					spawnMinion(new game::EntityMinionHeavy(), v[0], v[1]);

					delete[] v;
				}
			}
			break;
		case SDLK_DELETE:
			if(pressed && !getEditMode() && !game::physics::isPaused()){
				if(game::getSelectedMinion() != NULL){
					game::world::removeEntity(game::getSelectedMinion());
					game::setSelectedMinion(NULL);
				}
			}
			break;
		case SDLK_SPACE:
			if(pressed && !getEditMode()){
				for(game::EntityMinion *m : *game::world::getMinions()){
					m->doNextCommand();
				}
			}
			break;
		}
	}
}
void Controller::handleMouse(int key, float x, float y, bool pressed, Tool *currentTool, WidgetWorld *w){
	if(inputType == 0){
		if(key == 2){
			render::options::setZoom(1);
		}
		float* v = screenToWorldSpace(x, y);
		if(!getEditMode()){
			switch(key){
			case 1:
				if(pressed){
					bool foundOne;
					foundOne = false;
					for(game::EntityMinion *e : *game::world::getMinions()){
						if(game::physics::pointInRect(v[0], v[1], e->getX(), e->getY(), e->getWidth(), e->getHeight())){
							game::setSelectedMinion(e);
							foundOne = true;
						}
					}
					if(!foundOne){
						game::setSelectedMinion(NULL);
					}
				}
				break;
			case 3:

				game::EntityMinion *m = 0;
				for(game::EntityMinion *e : *game::world::getMinions()){
					if(game::physics::pointInRect(v[0], v[1], e->getX(), e->getY(), e->getWidth(), e->getHeight())){
						m = e;
						break;
					}
				}
				if(pressed && !w->hasChild(&commandSelector)){
					if(m != NULL && hypot(game::getPlayer()->getXCenter() - m->getXCenter(), game::getPlayer()->getYCenter() - m->getYCenter()) < 5){
						commandSelector.setLocation(x, y);
						game::setSelectedMinion(m);
						w->addWidget(&commandSelector);
						setFocus(&commandSelector);
						commandSelector.setCurrentMinion(m);
					}
				}

				break;
			}
		}
		else if(currentTool != NULL && w->isFocused()){
			if(pressed)
				currentTool->begin(v[0], v[1], key);
			else
				currentTool->stop(v[0], v[1], key);
		}

		delete[] v;
	}
}

void Controller::handleMouseWheel(float value){
	if(inputType == 0 && getEditMode()){
		if(value > 0){
			render::options::setZoom(render::options::getZoom() / 1.1);
		}
		else{
			render::options::setZoom(render::options::getZoom() * 1.1);
		}
	}
}

void Controller::handleStick(int axis, float value){
	if(axis < 4)
		axisValues[axis] = value;
	if(axis == 0){
		if(!getEditMode() && !game::physics::isPaused()){
			game::getPlayer()->setKeyMoveLeft(value < -0.1);
			game::getPlayer()->setKeyMoveRight(value > 0.1);
		}
		else{
			game::setScrollVelocityX(cameraSpeed * value);
		}
	}
	else if(axis == 1){
		if(!(!getEditMode() && !game::physics::isPaused())){
			game::setScrollVelocityY(-cameraSpeed * value);
		}
	}
	else if(axis == 2){
		if(axisValues[2] < 0.3 && axisValues[2] > -0.3 && axisValues[3] < 0.3 && axisValues[3] > -0.3){
			commandFree = true;
		}
		else if(commandFree){
			game::EntityMinion *m = getClosest();
			if(m != NULL){
				if(value > 0.7){
					commandFree = false;
					if(!altCommands)
						m->appendCommand(0); //moveRight
				}
				else if(value < -0.7){
					commandFree = false;
					if(!altCommands)
						m->appendCommand(1); //moveLeft
				}
			}
		}
	}
	else if(axis == 3){
		if(axisValues[2] < 0.3 && axisValues[2] > -0.3 && axisValues[3] < 0.3 && axisValues[3] > -0.3){
			commandFree = true;
		}
		else if(commandFree){
			game::EntityMinion *m = getClosest();
			if(m != NULL){
				if(value > 0.7){
					commandFree = false;
					if(!altCommands)
						m->appendCommand(4); //stop
					else
						m->appendCommand(-1); //clear
				}
				else if(value < -0.7){
					commandFree = false;
					if(!altCommands)
						m->appendCommand(2); //jump
					else
						m->appendCommand(3); //activate
				}
			}
		}
	}
	//	else if(axis == 3){
	//		mouseY += value * 0.1;
	//	}

}


void Controller::handleButton(int button, bool pressed){

	if(button == 0){
		if(pressed && !getEditMode() && !game::physics::isPaused()){
			if(game::getMinionCount() < 3){
				spawnMinion(new game::EntityMinion(), game::getPlayer()->getXCenter(), game::getPlayer()->getYCenter());
			}
		}
	}
	else if(button == 2){
		if(pressed && !getEditMode() && !game::physics::isPaused()){
			if(game::getMinionCount() < 3){
				spawnMinion(new game::EntityMinionClimber(), game::getPlayer()->getXCenter(), game::getPlayer()->getYCenter());
			}
		}
	}
	else if(button == 3){
		if(pressed && !getEditMode() && !game::physics::isPaused()){
			if(game::getSelectedMinion() != NULL){
				game::world::removeEntity(game::getSelectedMinion());
			}
		}
	}
	else if(button == 4){
		if(pressed){
			altCommands = true;
		}
		else{
			altCommands = false;
		}
	}
	else if(button == 5){
		if(pressed && !getEditMode()){
			for(game::EntityMinion *m : *game::world::getMinions()){
				m->doNextCommand();
			}
		}
	}
	else if(button == 7){
		if(pressed && !getEditMode()){
			int s = game::world::getMinions()->size();
			if(s > 0){
				selectedIndex ++;
				selectedIndex %= s;
				if(selectedIndex >= 0 && selectedIndex < s){
					game::setSelectedMinion(game::world::getMinions()->at(selectedIndex));
				}
			}
		}
	}
	else if(button == 6){
		if(pressed && !getEditMode()){
			int s = game::world::getMinions()->size();
			if(s > 0){
				selectedIndex --;
				//selectedIndex %= s;
				if(selectedIndex < 0)
					selectedIndex += s;
				if(selectedIndex >= 0 && selectedIndex < s){
					game::setSelectedMinion(game::world::getMinions()->at(selectedIndex));
				}
			}
		}
	}


}


void Controller::setPlayer(game::EntityPlayer *p){
	player = p;
}
} /* namespace gui */
