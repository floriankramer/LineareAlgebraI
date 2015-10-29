/*
 * Game.h
 *
 *  Created on: Dec 22, 2014
 *      Author: dwarf
 */

#ifndef GAME_H_
#define GAME_H_

#include "../Logger.h"
#include "EntityPlayer.h"
#include "os.h"
#include <string>
#ifdef windows
#include <windows.h>
#endif

#include "Widget.h"
#include "WidgetWorld.h"
#include "WidgetHomescreen.h"
namespace game{

extern Logger gameLogger;

void * run(void *);
void stop();
EntityPlayer * getPlayer();
void setEditMode(bool);
void setCameraPosition(float x, float y);
void setScrollVelocity(float x, float y);
void setScrollVelocityX(float x);
void setScrollVelocityY(float y);
float getCameraX();
float getCameraY();
std::string setTool(std::string);
std::string setToolParameter(std::string name, std::string value);

void toGameWidget();
void toHomescreenWidget();
bool isInHomescreen();

gui::WidgetWorld* getWidgetWorld();
gui::Widget* getMasterWidget();
gui::WidgetHomescreen *getHomescreenWidget();


/**
 * returns a number which is garuanteed to change every update, but which may not be larger than it was at the last update
 */
unsigned int getTickCount();

#ifdef windows
DWORD WINAPI winRun(LPVOID lpParam);
#endif

}


#endif /* GAME_H_ */
