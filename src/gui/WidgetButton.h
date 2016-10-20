/*
 * WidgetButton.h
 *
 *  Created on: Mar 22, 2015
 *      Author: dwarf
 */

#ifndef GUI_WIDGETBUTTON_H_
#define GUI_WIDGETBUTTON_H_

#include <Widget.h>
#include <string>

namespace warp{
namespace gui {

class ButtonListener{
public:
	ButtonListener(){}
	virtual ~ButtonListener(){}
	virtual void onButtonPressed(std::string text){}
};

class WidgetButton: public Widget {
public:
	WidgetButton(std::string text);
	virtual ~WidgetButton();
	void handleMouseButtonEvent(int button, float x, float y, bool pressed);
	void render(float updateFactor);
	void setListener(ButtonListener *l);
private:
	std::string text;
	ButtonListener *listener;
	bool pressed;
};

} /* namespace warp{
namespace gui */

}
#endif /* GUI_WIDGETBUTTON_H_ */
