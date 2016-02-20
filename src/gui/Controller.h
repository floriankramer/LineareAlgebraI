/*
 * Controller.h
 *
 *  Created on: Apr 15, 2015
 *      Author: dwarf
 */

#ifndef GUI_CONTROLLER_H_
#define GUI_CONTROLLER_H_

#include <ScreenWorld.h>
#include "EntityPlayer.h"
#include "Tool.h"
namespace gui {

class Controller {
public:
	Controller();
	virtual ~Controller();

	void handleKey(int key, bool pressed);
	void handleMouse(int key, float x, float y, bool pressed, Tool *currentTool, WidgetWorld *w);
	void handleStick(int axis, float value);
	void handleMouseWheel(float value);
	void handleButton(int button, bool pressed);
	void setPlayer(game::EntityPlayer *p);
	void render(float updateFactor);
private:
	game::EntityPlayer *player;
	float mouseX, mouseY;
	int inputType;
	bool commandFree;
	bool altCommands;
	float axisValues[4];
	int selectedIndex;
};

} /* namespace gui */

#endif /* GUI_CONTROLLER_H_ */
