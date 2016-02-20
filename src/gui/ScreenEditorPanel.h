/*
 * WidgetEditorPanel.h
 *
 *  Created on: Apr 27, 2015
 *      Author: dwarf
 */

#ifndef GUI_WIDGETEDITORPANEL_H_
#define GUI_WIDGETEDITORPANEL_H_

#include <Widget.h>
#include <WidgetButton.h>

namespace gui {

class WidgetEditorPanel: public Widget, public ButtonListener {
public:
	WidgetEditorPanel();
	virtual ~WidgetEditorPanel();
	void render(float updateFactor);
};

} /* namespace gui */

#endif /* GUI_WIDGETEDITORPANEL_H_ */
