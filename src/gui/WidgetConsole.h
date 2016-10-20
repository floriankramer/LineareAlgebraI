/*
 * WidgetConsole.h
 *
 *  Created on: Dec 31, 2014
 *      Author: dwarf
 */

#ifndef GUI_WIDGETCONSOLE_H_
#define GUI_WIDGETCONSOLE_H_

#include "Widget.h"
#include <string>

namespace warp{
namespace gui {
const int numLines = 10;

class WidgetConsole: public gui::Widget {
public:
	WidgetConsole();
	virtual ~WidgetConsole();
	void render(float updateFactor);
	void handleMouseButtonEvent(int, float, float, bool);
	void handleKeyEvent(int, int, bool);
	void handleTextEvent(std::string s);
	virtual void onFocusChanged();
private:
	float left, bottom;
	int blinkState;
	std::string lines[numLines];
	unsigned int cursorPos, historyPos;


};

} /* namespace game */

}
#endif /* GUI_WIDGETCONSOLE_H_ */
