/*
 * WidgetWorld.cpp
 *
 *  Created on: Dec 23, 2014
 *      Author: dwarf
 */

#include "WidgetWorld.h"
#include "Renderer.h"
#include "GraphicsOptions.h"
#include "World.h"
#include "Vec2f.h"
#include "Warp.h"
#include "SDL2/SDL_keycode.h"
#include "EntityMinion.h"
#include "CollisionHandler.h"
#include "Game.h"
#include "Mouse.h"
#include "LightHandler.h"

#include "WidgetTextureSelect.h"
#include "WidgetMinionCommandSelector.h"

#include "CoordinateConverter.h"
#include "CommandHandler.h"
#include "Controller.h"

#include "ToolBrush.h"
#include "ToolLight.h"
#include "ToolNode.h"
#include "ToolTexture.h"
#include "ToolWorld.h"

#include "Physics.h"
#include <string>

#include "EntityBox.h"

namespace gui {

void handleToolSelect(std::string s, void *data);

ToolBrush brush;
ToolLight light;
ToolNode node;
ToolTexture texture;
ToolWorld world;

WidgetTextureSelect textureSelector(&texture);
WidgetRadialMenu minionSpawnMenu;

Controller playerController;

render::Color colorWorldOverlay(0.7, 0.7, 0.7, 0.5);

//std::string toolOptions[] = {"brush", "texture", "light", "node"};

void WidgetWorld::setupMinonCommandRadialMenu(){

	RadialMenuTree root;
	render::loadTexture("minionMoveL", "./gfx/gui/button_move_l.png");
	render::loadTexture("minionMoveR", "./gfx/gui/button_move_r.png");
	render::loadTexture("minionJump", "./gfx/gui/jump.png");
	render::loadTexture("minionActivate", "./gfx/gui/activate.png");
	render::loadTexture("minionStop", "./gfx/gui/button_stop.png");
	render::loadTexture("minionSpecial", "./gfx/gui/minionSpecial.png");

	RadialMenuTree mRight("move right");
	mRight.setTexture("minionMoveR", 0.1, 0.1);
	RadialMenuTree mLeft("move left");
	mLeft.setTexture("minionMoveL", 0.1, 0.1);
	RadialMenuTree jump("jump");
	jump.setTexture("minionJump", 0.1, 0.1);
	RadialMenuTree activate("activate");
	activate.setTexture("minionActivate", 0.1, 0.1);

	RadialMenuTree tree1("1");
	tree1.add(mRight);
	tree1.add(mLeft);
	tree1.add(jump);
	tree1.add(activate);

	RadialMenuTree tree2("2");
	tree2.add(mRight);
	tree2.add(mLeft);
	tree2.add(jump);
	tree2.add(activate);

	RadialMenuTree tree3("3");
	tree3.add(mRight);
	tree3.add(mLeft);
	tree3.add(jump);
	tree3.add(activate);

	root.add(tree1);
	root.add(tree2);
	root.add(tree3);

	commandSelectMenu.setOptions(root);


	RadialMenuTree root2;
	root2.add(RadialMenuTree("default"));
	root2.add(RadialMenuTree("jumper"));
	root2.add(RadialMenuTree("climber"));
	minionSpawnMenu.setOptions(root2);

}

WidgetWorld::WidgetWorld() : Widget(), buttonReloadWorld("reset"){
	this->setWidth(2 * render::options::getAspectRatio());
	this->setHeight(2);
	this->setListenToMouse(true);
	this->setListenToKeys(true);
	this->setLocation(0, 0);
	this->tool = &brush;

	RadialMenuTree root;
	root.add(RadialMenuTree("brush"));
	root.add(RadialMenuTree("texture"));
	root.add(RadialMenuTree("light"));
	root.add(RadialMenuTree("node"));
	root.add(RadialMenuTree("world"));


	toolSelectMenu.setOptions(root);
	toolSelectMenu.setCallbackFunction(handleToolSelect, (void*)this);

	guiLogger.log("setting up widget world");
	setupMinonCommandRadialMenu();
	commandPosX = 0;
	commandPosY = 0;


	textureSelector.setWidth(0.46);
	textureSelector.setHeight(0.46);
	textureSelector.setLocation(render::options::getAspectRatio() - 0.6, 0.4);

	buttonReloadWorld.setWidth(0.2);
	buttonReloadWorld.setHeight(0.1);
	buttonReloadWorld.setLocation(render::options::getAspectRatio() - 0.2, -0.8);
	buttonReloadWorld.setListener(this);
	addWidget(&buttonReloadWorld);

	editorPanel.setWidth(0.5);
	editorPanel.setHeight(2);
	editorPanel.setLocation(-render::options::getAspectRatio() + 0.25, 0);

}

WidgetWorld::~WidgetWorld() {

}

void WidgetWorld::onButtonPressed(std::string text){
	if(text == "reset"){
		game::world::reloadWorld();
		if(!getEditMode())
			game::physics::setPaused(false);
	}
}


void WidgetWorld::displayMinionSpawnMenu(float x, float y){
	if(!hasChild(&minionSpawnMenu)){
		addWidget(&minionSpawnMenu);
		minionSpawnMenu.setLocation(x, y);
	}
}

std::string WidgetWorld::removeMinionSpawnMenu(){
	removeWidget(&minionSpawnMenu);
	return minionSpawnMenu.getSelectedOption();
}

void WidgetWorld::render(float updateFactor){

	render::setRenderTarget(render::WORLD);

	if(getEditMode() && tool != &node)
		game::world::renderNodes(getEditMode(), updateFactor);

	game::world::renderBlocks(updateFactor);
	if(getEditMode() && tool == &light)
		render::light::render(updateFactor);
	if(!getEditMode())
		game::world::renderNodes(getEditMode(), updateFactor);

	game::world::renderEntities(updateFactor);
	game::getPlayer()->render(updateFactor);

	if(getEditMode() && tool == &node)
		game::world::renderNodes(getEditMode(), updateFactor);
	game::world::renderNodeConnections(getEditMode(), updateFactor);

	if(getEditMode()){
		game::world::renderWorldOutlines(updateFactor);
		tool->render(updateFactor);
	}




	//	render::drawCurve(0, 5, 0, 0, 0, 0, 1, -1, .1);

	render::setRenderTarget(render::HUD);
	renderHud();
	playerController.render(updateFactor);
	Widget::render(updateFactor);

	bool hasFocus = isFocused();
	std::vector<Widget*> c;
	c.push_back(this);
	while(!c.empty() && !hasFocus){
		Widget *w = c[c.size() - 1];
		c.pop_back();
		if(w->isFocused()){
			hasFocus = true;
		}
		else{
			for(Widget *w2 : *w->getChildren()){
				c.push_back(w2);
			}
		}
	}
	if(!hasFocus){
		render::drawRect(0, 0, render::options::getAspectRatio() * 2, 2, 0, colorWorldOverlay);
	}
}

void WidgetWorld::handleMouseButtonEvent(int k, float x, float y, bool pressed){
	playerController.handleMouse(k, x + getLeft(), y + getBottom(), pressed, tool, this);
	if(k == 2 && pressed){ //TODO remove
		game::EntityBox *b = new game::EntityBox(2, 2);
		float *f = screenToWorldSpace(x, y);
		b->setPositionCenter(f[0], f[1]);
		game::world::spawnEntity(b);
		delete[] f;
	}
}

void handleToolSelect(std::string s, void *data){
	((WidgetWorld*)data)->setTool(s);
}

std::string WidgetWorld::setTool(std::string s){
	std::string answer = "tools: brush, light, node, texture, world";
	removeWidget(&textureSelector);
	if(s == "brush"){
		tool = &brush;
		answer = "brush selected";
	}
	else if(s == "light"){
		tool = &light;
		answer = "light selected";
	}
	else if(s == "node"){
		tool = &node;
		answer = "node selected";
	}
	else if(s == "texture"){
		tool = &texture;
		answer = "texture selected";
		addWidget(&textureSelector);
	}
	else if(s == "world"){
		tool = &world;
		answer = "world selected";
	}
	return answer;
}

std::string WidgetWorld::setToolParameter(std::string name, std::string value){
	if(tool != NULL){
		return tool->setParameter(name, value);
	}
	return "no tool selected";
}

void WidgetWorld::handleMouseMotionEvent(float x, float y){
	if(getEditMode()){
		float *w = screenToWorldSpace(x + getLeft(), y + getBottom());
		if(isMouseButtonDown(1) && tool != NULL){
			tool->step(w[0], w[1], 1);
		}
		if(isMouseButtonDown(2) && tool != NULL){
			tool->step(w[0], w[1], 2);
		}
		else if(isMouseButtonDown(3) && tool != NULL){
			tool->step(w[0], w[1], 3);
		}
		delete[] w;
	}
}

void WidgetWorld::handleKeyEvent(int k, int mod, bool pressed){
	playerController.handleKey(k, pressed);

	if(getEditMode() && tool != NULL){
		tool->onKeyPress(k, mod, pressed, this);
	}
	switch(k){
	case SDLK_q:
		if(getEditMode()){
			if(pressed){
				if(!hasChild(&toolSelectMenu)){
					float *f = getMousePosition();
					toolSelectMenu.setLocation(f[0], f[1]);
					this->addWidget(&toolSelectMenu);
				}
			}
			else{
				setTool(toolSelectMenu.getSelectedOption());
				this->removeWidget(&toolSelectMenu);
			}
		}
		break;
	case SDLK_TAB: //TODO remove in release!!!
		if(!pressed){
			if(getEditMode()){
				handleCommand("setEditMode a");
			}
			else{
				handleCommand("setEditMode");
			}
			break;
		}
	}

}

void WidgetWorld::handleAxisEvent(int axis, float value){
	playerController.handleStick(axis, value);
}

void WidgetWorld::handleButtonEvent(int button, bool pressed){
	playerController.handleButton(button, pressed);
}

void WidgetWorld::handleMouseWheelEvent(float value){
	playerController.handleMouseWheel(value);
}

void WidgetWorld::onEditModeChanged(bool b){
	if(b){
		if(!hasChild(&editorPanel)){
			addWidget(&editorPanel);
		}
	}
	else{
		removeWidget(&editorPanel);
	}
}

void WidgetWorld::renderHud(){
	for(int i = 0; i < 3; i++){
		render::drawRectOutline(-render::options::getAspectRatio() + 0.1f, -0.9f + 0.15f * i, 0.1f, 0.1f, 0, 0.005f);
		if(i < 3 - game::getMinionCount()){
			render::drawSprite(-render::options::getAspectRatio() + 0.1f, -0.9f + 0.15f * i, 0.09f, 0.09f, 0, "minion_default");
		}
	}
	//float x = 0.9;
	//	render::drawRect(0, -.9f, 1 * 0.5f / 1, 0.05f, 0);
	//	render::drawString(x, 0.8 - 0, 0.05, std::string("Use WASD to move around"));
	//	render::drawString(x, 0.8 - 0.05, 0.05, std::string("Press C to create a Minion"));
	//	render::drawString(x, 0.8 - 0.10, 0.05, std::string("Select Minions with LMB"));
	//	render::drawString(x, 0.8 - 0.15, 0.05, std::string("Give Them Orders with RMB"));
	//	render::drawString(x, 0.8 - 0.20, 0.05, std::string("Select Jump Order with J"));
	//	render::drawString(x, 0.8 - 0.25, 0.05, std::string("Select Move Order with M"));
	//	render::drawString(x, 0.8 - 0.30, 0.05, std::string("Use DEL to delete selected Minion"));
	//	render::drawString(x, 0.8 - 0.35, 0.05, std::string("Close the demo with ESC"));

	if(getEditMode()){
		//		render::drawString(-render::options::getAspectRatio() + 0.05, 0.9, 0.05, std::string("Tool: ") + std::string(tool->getName()));
		render::drawString(render::options::getAspectRatio() - 0.5, 0.9, 0.05, std::string("Tool: ") + std::string(tool->getName()));
	}
}


} /* namespace game */
