/*
 * Game.cpp
 *
 *  Created on: Dec 22, 2014
 *      Author: dwarf
 */

#include "Game.h"

#include "World.h"

#include "TextureLoader.h"
#include "Renderer.h"
#include "GraphicsOptions.h"
#include "main.h"
#include "os.h"

#include "Mouse.h"
#include "Vec2f.h"
#include "ResourceLoader.h"

#include "Entity.h"
#include "EntityMinion.h"
#include "Keyboard.h"
#include "Physics.h"
#include "CommandHandler.h"
#include "AnimationLoader.h"

#include "AudioPlayer.h"

#include <cmath>
#include <iostream>

#include <fstream>

namespace game{

Logger gameLogger("Game");

void synchronizeRenderables();
void updateCamera();

bool inHomescreen = true;
bool shouldRun = true;
gui::Widget *masterWidget;
gui::WidgetWorld *worldWidget;
gui::WidgetHomescreen *homescreenWidget;

EntityPlayer *player;

float scrollX = 0, scrollY = 0, scrollSpeedX = 0, scrollSpeedY = 0, scrMidX, scrMidY, screenDistanceX, screenDistanceY;
float scrollVX = 0, scrollVY = 0;

bool editMode = false;

unsigned int tickCount = 0;


#ifdef windows
DWORD WINAPI winRun(LPVOID lpParam){
	std::cout << "starting game thread" << std::endl;
	run(NULL);
	return 0;
}
#endif

void setEditMode(bool b){
	editMode = b;
}

void toGameWidget(){
	masterWidget->removeWidget(homescreenWidget);
	masterWidget->addWidget(worldWidget);
	gui::setFocus(worldWidget);
	inHomescreen = false;

}
void toHomescreenWidget(){
	gui::handleCommand("setPaused true");
	masterWidget->removeWidget(worldWidget);
	masterWidget->addWidget(homescreenWidget);
	gui::setFocus(homescreenWidget);
	inHomescreen = true;
}
bool isInHomescreen(){
	return inHomescreen;
}

void *run(void* args){

	gameLogger.setName("game");
	gui::guiLogger.setName("gui");

	audio::init();
	audio::loadSound("sfx/out_of_reach.wav", "out_of_reach");
	audio::loadSound("sfx/temp/click.ogg", "click");
	audio::loadSound("sfx/temp/death.wav", "death");
	audio::loadSound("sfx/temp/stone_slide.wav", "stone_slide");
	//audio::loadSound("sfx/temp/pippin_the_hunchback.wav", "pippin");
	audio::setMusicQueueLoop(true);
	audio::queueMusic("sfx/temp/pippin_the_hunchback.ogg");
//	audio::queueMusic("sfx/temp/moorland_short.wav");
	//audio::playPlainSound("pippin", 0.5);

	gui::initKeyboard();

	//	masterWidget->addWidget(&worldWidget);
	//	gui::setFocus(&worldWidget);

	loadResourceFile("./gfx/base");
	render::animation::loadSCMLFile("char/animation.scml", "char/");

	masterWidget = new gui::Widget();
	homescreenWidget = new gui::WidgetHomescreen();
	worldWidget = new gui::WidgetWorld();

	masterWidget->addWidget(homescreenWidget);
	gui::setFocus(homescreenWidget);

	gameLogger.log("Initializing Game Logic");

	player = new EntityPlayer();

	//world::loadWorld(toOsFileString("./level/lvl1")); //"/home/dwarf/jworkspace/_Game/data/lvl1"
	physics::setPaused(true);

	//TODO remove this part of the code, which is doing auto setup on launch for debugging
	//	gui::handleCommand("loadWorld ./level/lvl1");
	//	gui::handleCommand("setEditMode a");
	//	gui::handleCommand("setLightEnabled a");

	while(shouldRun){


		tickCount ++;

		masterWidget->handleInput();

		physics::update();
		updateCamera();
		audio::updateStreams();

		synchronizeRenderables();

	}

	//	for(Entity *e : *world::getEntities()){
	//		delete e;
	//	}

	world::freeMemory();

	gui::destroyKeyboard();
	audio::close();

	delete masterWidget;
	delete worldWidget;
	delete homescreenWidget;

	return 0;
}

void synchronizeRenderables(){
	physicsThread.wait();

	render::resetRenderables();

	masterWidget->render(render::getUpdateFactor());

	while(!renderThread.isWaiting() && shouldRun){
		SDL_Delay(1);
	}
	renderThread.wakeUp();
}

void updateCamera(){
	if(!editMode){
		scrMidX = player->getXCenter();
		scrMidY = player->getYCenter();
		screenDistanceX = scrMidX - scrollX;
		screenDistanceY = scrMidY - scrollY;
		int dX = screenDistanceX / fabs(screenDistanceX);
		int dY = screenDistanceY / fabs(screenDistanceY);

		if (fabs(screenDistanceX) > 5) {
			scrollSpeedX = dX * 0.2 * (fabs(screenDistanceX) - 5) * physics::getSimulationSpeed(); //(pow(screenDistanceX, 3))
		} else {
			scrollSpeedX = 0;
		}

		if (fabs(screenDistanceY) > 5) {
			scrollSpeedY = dY * 0.2 * (fabs(screenDistanceY) - 5) * physics::getSimulationSpeed(); //(pow(screenDistanceY , 3))
		} else {
			scrollSpeedY = 0;
		}

		scrollX += scrollSpeedX;
		scrollY += scrollSpeedY;
	}
	else{
		scrollX += scrollVX;
		scrollY += scrollVY;
	}


	render::setCameraPosition(scrollX, scrollY);
}

void setScrollVelocity(float x, float y){
	scrollVX = x;
	scrollVY = y;
}

void setScrollVelocityX(float x){
	scrollVX = x;
}

void setScrollVelocityY(float y){
	scrollVY = y;
}

void setCameraPosition(float x, float y){
	scrollX = x;
	scrollY = y;
}

void stop(){
	shouldRun = false;
}

float getCameraX(){
	return scrollX;
}

float getCameraY(){
	return scrollY;
}

EntityPlayer *getPlayer(){
	return player;
}

std::string setTool(std::string s){
	return worldWidget->setTool(s);
}

std::string setToolParameter(std::string name, std::string value){
	return worldWidget->setToolParameter(name, value);
}

unsigned int getTickCount(){
	return tickCount;
}

gui::WidgetWorld* getWidgetWorld(){
	return worldWidget;
}

gui::WidgetHomescreen* getHomescreenWidget(){
	return homescreenWidget;
}


gui::Widget* getMasterWidget(){
	return masterWidget;
}

}


