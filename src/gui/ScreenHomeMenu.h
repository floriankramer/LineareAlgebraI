/*
 * WidgetHomescreenMenu.h
 *
 *  Created on: Aug 26, 2015
 *      Author: dwarf
 */

#ifndef SCREENHOMEMENU_H_
#define SCREENHOMEMENU_H_

#include "Widget.h"
#include "WidgetButton.h"

namespace gui {

class WidgetHomescreenMenu: public Widget, public ButtonListener {
public:
	WidgetHomescreenMenu();
	virtual ~WidgetHomescreenMenu();
	void render(float updateFactor);
	void onButtonPressed(std::string text);
private:
	WidgetButton buttonStart;
	WidgetButton buttonEditor;
	WidgetButton buttonOptions;
	WidgetButton buttonExit;
};

} /* namespace gui */

#endif /* SCREENHOMEMENU_H_ */
