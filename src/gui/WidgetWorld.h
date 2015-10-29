/*
 * WidgetWorld.h
 *
 *  Created on: Dec 23, 2014
 *      Author: dwarf
 */

#ifndef GUI_WIDGETWORLD_H_
#define GUI_WIDGETWORLD_H_

#include "Widget.h"
#include "WidgetConsole.h"
#include "Tool.h"
#include <string>
#include "WidgetRadialMenu.h"
#include "WidgetButton.h"
#include "WidgetEditorPanel.h"

namespace gui {

class WidgetWorld: public gui::Widget, ButtonListener {
public:
	WidgetWorld();
	virtual ~WidgetWorld();
	void render(float updateFactor);
	void handleMouseButtonEvent(int button, float x, float y, bool pressed);
	void handleKeyEvent(int, int, bool);
	void handleMouseMotionEvent(float, float);
	void displayMinionSpawnMenu(float x, float y);
	std::string removeMinionSpawnMenu();
	std::string setTool(std::string);
	std::string setToolParameter(std::string name, std::string value);

	virtual void onButtonPressed(std::string text);
	virtual void onEditModeChanged(bool b);
	virtual void handleAxisEvent(int axis, float value);
	virtual void handleButtonEvent(int button, bool pressed);
	virtual void handleMouseWheelEvent(float value);

private:
	Tool *tool;
	void renderHud();
	WidgetRadialMenu toolSelectMenu;
	WidgetRadialMenu commandSelectMenu;
	float commandPosX, commandPosY;

	void setupMinonCommandRadialMenu();

	WidgetButton buttonReloadWorld;
	WidgetEditorPanel editorPanel;
};

} /* namespace game */

#endif /* GUI_WIDGETWORLD_H_ */
