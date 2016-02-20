/*
 * WidgetMinionCommandSelector.cpp
 *
 *  Created on: Mar 10, 2015
 *      Author: dwarf
 */

#include <ScreenMinionCommandSelector.h>
#include "Renderer.h"
#include "CollisionHandler.h"
#include "SDL2/SDL_keycode.h"
#include "Mouse.h"
#include <cmath>

namespace gui {

render::Color backgroundColor(0.7, 0.7, 0.7, 0.8);
render::Color colorBox(0, 0, 0);

std::string commandIcons[] = {"minionMoveR", "minionMoveL", "minionJump", "minionActivate", "minionStop", "minionSpecial"};

const float rectSize = 0.9, rectSpace = 1.2;
const float iconSize = 0.75, iconSpace = 1, iconTopOffset = 1.4;
const float iconCentralShift = (rectSize - iconSize) / 2;

WidgetMinionCommandSelector::WidgetMinionCommandSelector() : Widget() {
	setWidth(6);
	setHeight(2.5);
	setListenToMouse(true);
	setListenToKeys(true);
	grabbedCommand = -1;
	grabbedX = 0;
	grabbedY = 0;
	minion = 0;
	//setPaddingX(0.05);
}

WidgetMinionCommandSelector::~WidgetMinionCommandSelector() {

}

void WidgetMinionCommandSelector::setCurrentMinion(game::EntityMinion *e){
	minion = e;
	posFactors.resize(game::getCommandDepth());
	animPos.resize(game::getCommandDepth());
	commands.resize(game::getCommandDepth());
	for(int i = 0; i < game::getCommandDepth(); i++){
		posFactors[i] = 1;
		animPos[i] = 1;
		commands[i] = minion->getCommand(i);
	}
}

void WidgetMinionCommandSelector::onFocusChanged(){
	if(!isFocused())
		if(getParent() != NULL){
			getParent()->removeWidget(this);
		}
}


void WidgetMinionCommandSelector::render(float updateFactor){
	renderer->drawRect(0, 0, getWidth(), getHeight(), 0, backgroundColor);

	//update respawn animations for commands
	for(int i = 0; i < numCommands; i++){
		if(animPos[i] < 1){
			animPos[i] += updateFactor * 4;
			posFactors[i] = sin(animPos[i] * 3.1415 / 2);
			if(animPos[i] > 1)
				animPos[i] = 1;
		}
	}

	//draw upper line of commands
	for(int i = 0; i < game::getCommandDepth(); i++){
		renderer->drawRectOutline(i * rectSpace, 0, rectSize, rectSize, 0, 0.05, colorBox);
		if(commands[i] != -1){
			renderer->drawSprite(i * rectSpace + iconCentralShift, iconCentralShift, iconSize, iconSize, 0, commandIcons[commands[i]]);
		}
	}

	//draw lower line of available commands
	for(int i = 0; i < numCommands; i++){
		if(i != grabbedCommand){	
			renderer->drawSprite(i * iconSpace, iconTopOffset, iconSize * posFactors[i], iconSize * posFactors[i], 0, commandIcons[i]);
		}


	}

	if(grabbedCommand != -1)
		renderer->drawSprite(grabbedX, grabbedY, iconSize, iconSize, 0, commandIcons[grabbedCommand]);

}

void WidgetMinionCommandSelector::handleMouseButtonEvent(int button, float x, float y, bool pressed){
	if(button == 1){	
		if(pressed){
			for(int i = 0; i < numCommands; i++){ //check for command pickup in lower line
				if(game::physics::pointInRect(x, y, i * iconSpace, iconTopOffset, iconSize, iconSize)){
					grabbedCommand = i;
					grabbedX = x - iconSize / 2;
					grabbedY = y - iconSize / 2;
					return;
				}
			}
			for(int i = 0; i < game::getCommandDepth(); i++){ //check for command deletion in upper line
				if(game::physics::pointInRect(x, y, i * rectSpace, 0 , rectSize, rectSize)){
					commands[i] = -1;
					if(minion != NULL){
						minion->setCommand(i, -1);
					}
					return;
				}
			}
		}
		else{
			if(grabbedCommand != -1){
				for(int i = 0; i < game::getCommandDepth(); i++){ //check for command setting in upper line
					if(game::physics::pointInRect(x, y, i * rectSpace, 0, rectSize, rectSize)){
						commands[i] = grabbedCommand;
						if(minion != NULL){
							minion->setCommand(i, grabbedCommand);
						}
					}
				}
				animPos[grabbedCommand] = 0;
				grabbedCommand = -1;
			}
		}
	}
}

void WidgetMinionCommandSelector::handleMouseMotionEvent(float x, float y){
	if(grabbedCommand != -1){
		grabbedX = x - iconSize / 2;
		grabbedY = y - iconSize / 2;
	}
}

void WidgetMinionCommandSelector::handleKeyEvent(int k, int mod, bool pressed){
	if(pressed){
		int t = -1;

		if(k == SDLK_1){
			t = 0;
		}
		else if(k == SDLK_2){
			t = 1;
		}
		else if(k == SDLK_3){
			t = 2;
		}

		if(t != -1){	
			float *f = getMousePosition();
			for(int i = 0; i < numCommands; i++){
				if(game::physics::pointInRect(f[0] - getAbsoluteLeft(), f[1] - getAbsoluteTop(), i * iconSpace, iconTopOffset, iconSize, iconSize)){
					commands[t] = i;
					if(minion != NULL){
						minion->setCommand(t, i);
					}
					animPos[i] = 0;
					return;
				}
			}
		}
	}
}

} /* namespace game */
