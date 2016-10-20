/*
 * WidgetSlider.cpp
 *
 *  Created on: Oct 27, 2015
 *      Author: dwarf
 */

#include <WidgetSlider.h>
#include "Mouse.h"

namespace warp{
namespace gui {

WidgetSlider::WidgetSlider() : Widget(), value(0) {
	setWidth(5);
	setHeight(1);
	setListenToMouse(true);

}

WidgetSlider::~WidgetSlider() {

}

void WidgetSlider::render(float upateFactor){
	float halfheight = getHeight() / 2;
	renderer->drawLine(0, halfheight, getWidth(), halfheight, 0.1);
	renderer->drawRect(value * getWidth() - 0.1, halfheight - 0.1, 0.2, 0.2, 0); //value * getWidth()
}

void WidgetSlider::onFocusChanged(){
	guiLogger.log("slider focus: %s", std::to_string(isFocused()).c_str());
}

void WidgetSlider::handleMouseButtonEvent(int button, float x, float y, bool pressed){
	if(button == 1 && pressed){
		value = x / getWidth();
	}
}

void WidgetSlider::handleMouseMotionEvent(float x, float y){
	if(isMouseButtonDown(1)){
		value = x / getWidth();
	}
}

void WidgetSlider::setValue(float v){
	value = v;
}

float WidgetSlider::getValue(){
	return value;
}

} /* namespace warp{
namespace gui */
}
