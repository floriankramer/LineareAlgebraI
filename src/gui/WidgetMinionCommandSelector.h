/*
 * WidgetMinionCommandSelector.h
 *
 *  Created on: Mar 10, 2015
 *      Author: dwarf
 */

#ifndef GUI_WIDGETMINIONCOMMANDSELECTOR_H_
#define GUI_WIDGETMINIONCOMMANDSELECTOR_H_

#include <Widget.h>
#include "EntityMinion.h"

namespace gui {

const int numCommands = 6;

class WidgetMinionCommandSelector : public Widget {
public:
	WidgetMinionCommandSelector();
	virtual ~WidgetMinionCommandSelector();
	void render(float updateFactor);
	void onFocusChanged();
	virtual void handleMouseButtonEvent(int button, float x, float y, bool pressed);
	virtual void handleMouseMotionEvent(float x, float y);
	void setCurrentMinion(game::EntityMinion *e);
	void handleKeyEvent(int k, int mod, bool pressed);
private:
	int grabbedCommand;
	float grabbedX, grabbedY;
	std::vector<int> commands;
	std::vector<float> animPos;
	std::vector<float> posFactors;
	game::EntityMinion *minion;
};

} /* namespace game */

#endif /* GUI_WIDGETMINIONCOMMANDSELECTOR_H_ */
