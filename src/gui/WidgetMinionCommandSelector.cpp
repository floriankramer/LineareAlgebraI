/*
 * WidgetMinionCommandSelector.cpp
 *
 *  Created on: Mar 10, 2015
 *      Author: dwarf
 */

#include <WidgetMinionCommandSelector.h>
#include "Renderer.h"
#include "CollisionHandler.h"
#include "SDL2/SDL_keycode.h"
#include "Mouse.h"
#include <cmath>

namespace gui {

render::Color backgroundColor(0.7, 0.7, 0.7, 0.8);
render::Color colorBox(0, 0, 0);

std::string commandIcons[] = {"minionMoveR", "minionMoveL", "minionJump", "minionActivate", "minionStop", "minionSpecial"};

const float rectSize = 0.09, rectSpace = 0.12;
const float iconSize = 0.075, iconSpace = 0.1, iconTopOffset = 0.2;
const float iconCentralShift = (rectSize - iconSize) / 2;

WidgetMinionCommandSelector::WidgetMinionCommandSelector() : Widget() {
	setWidth(0.6);
	setHeight(0.25);
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
		if(getParents()->size() > 0){
			Widget *p = getParents()->at(0);
			p->removeWidget(this);
		}
}


void WidgetMinionCommandSelector::render(float updateFactor){
	renderer.drawRect(0, 0, getWidth(), getHeight(), 0, backgroundColor);
	float height = getHeight();

	for(int i = 0; i < numCommands; i++){
		if(animPos[i] < 1){
			animPos[i] += updateFactor * 4;
			posFactors[i] = sin(animPos[i] * 3.1415 / 2);
			if(animPos[i] > 1)
				animPos[i] = 1;
		}
	}

	for(int i = 0; i < game::getCommandDepth(); i++){
		renderer.drawRectOutline(0.05 + i * rectSpace, height - 0.12, rectSize, rectSize, 0, 0.005, colorBox);
		if(commands[i] != -1){
			renderer.drawSprite(0.05 + i * rectSpace + iconCentralShift, height - 0.12 + iconCentralShift, iconSize, iconSize, 0, commandIcons[commands[i]]);
		}
	}

	for(int i = 0; i < numCommands; i++){
		if(i != grabbedCommand){
			//renderer.drawSprite(0.05 + i * iconSpace, height - iconTopOffset, iconSize, iconSize, 0, commandIcons[i]);
			renderer.drawSpriteCentered(0.08 + i * iconSpace, height - iconTopOffset, iconSize * posFactors[i], iconSize * posFactors[i], 0, commandIcons[i]);
		}


	}

	if(grabbedCommand != -1)
		renderer.drawSprite(grabbedX, grabbedY, iconSize, iconSize, 0, commandIcons[grabbedCommand]);

}

void WidgetMinionCommandSelector::handleMouseButtonEvent(int button, float x, float y, bool pressed){
	if(button == 1){
		float height = getHeight();
		if(pressed){
			for(int i = 0; i < numCommands; i++){
				if(game::physics::pointInRect(x, y, 0.08 + i * iconSpace - iconSize / 2, height - iconTopOffset - iconSize / 2, iconSize, iconSize)){
					grabbedCommand = i;
					grabbedX = x - iconSize / 2;
					grabbedY = y - iconSize / 2;
					return;
				}
			}
			for(int i = 0; i < game::getCommandDepth(); i++){
				if(game::physics::pointInRect(x, y, 0.05 + i * rectSpace, height - 0.12, rectSize, rectSize)){
					commands[i] = -1;
					return;
				}
			}
		}
		else{
			if(grabbedCommand != -1){
				for(int i = 0; i < game::getCommandDepth(); i++){
					if(game::physics::pointInRect(x, y, 0.05 + i * rectSpace, height - 0.12, rectSize, rectSize)){
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
			float height = getHeight();
			float *f = getMousePosition();
			for(int i = 0; i < numCommands; i++){
				if(game::physics::pointInRect(f[0] - getLeft(), f[1] - getBottom(), 0.08 + i * iconSpace - iconSize / 2, height - iconTopOffset - iconSize / 2, iconSize, iconSize)){
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
