/*
 * WidgetHomescreen.h
 *
 *  Created on: Mar 22, 2015
 *      Author: dwarf
 */

#ifndef GUI_WIDGETHOMESCREEN_H_
#define GUI_WIDGETHOMESCREEN_H_

#include <Widget.h>
#include "WidgetButton.h"
#include "WidgetHomescreenMenu.h"
#include "WidgetWorldSelection.h"
#include "WidgetOptions.h"
#include <string>

namespace gui {

class WidgetHomescreen: public Widget {
public:
	WidgetHomescreen();
	virtual ~WidgetHomescreen();
	void render(float updateFactor);
	void toMainMenu();
	void toWorldSelection();
	void toOptions();
private:
	WidgetHomescreenMenu homescreenMenu;
	WidgetWorldSelection worldSelection;
	WidgetOptions options;


};

} /* namespace gui */

#endif /* GUI_WIDGETHOMESCREEN_H_ */
