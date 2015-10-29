/*
 * WidgetToggle.cpp
 *
 *  Created on: Aug 26, 2015
 *      Author: dwarf
 */

#include <WidgetToggle.h>

namespace gui {

const render::Color widgetToggleTrue(0, 1, 0);
const render::Color widgetToggleFalse(1, 0, 0);

WidgetToggle::WidgetToggle(std::string name, bool initState) : Widget(),state(initState), name(name), listener(NULL) {
	setListenToMouse(true);
	setWidth(0.1);
	setHeight(0.1);

}

WidgetToggle::~WidgetToggle() {

}

void WidgetToggle::render(float updateFactor){
	renderer.drawRect(0, 0, getWidth(), getHeight(), 0, state ? widgetToggleTrue : widgetToggleFalse);
	Widget::render(updateFactor);
}

void WidgetToggle::handleMouseButtonEvent(int button, float x, float y, bool pressed){
	if(!pressed){
		state = !state;
		if(listener != NULL){
			listener->onToggled(name, state);
		}
	}
}

void WidgetToggle::setListener(ToggleListener* l){
	listener = l;
}

void WidgetToggle::setState(bool s){
	state = s;
}

} /* namespace gui */
