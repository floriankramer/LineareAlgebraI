/*
 * WidgetButton.cpp
 *
 *  Created on: Mar 22, 2015
 *      Author: dwarf
 */

#include <WidgetButton.h>
#include "Renderer.h"
#include "AudioPlayer.h"

namespace gui {

render::Color colorButtonFont(0, 0, 0);
render::Color colorButtonBackground(0.7, 0.7, 0.7);
render::Color colorButtonBackgroundPressed(0.6, .6, .6);

WidgetButton::WidgetButton(std::string text) : Widget(){
	listener = 0;
	this->text = text;
	setListenToMouse(true);
	pressed = false;
	setWidth(0.4);
	setHeight(0.1);
}

WidgetButton::~WidgetButton(){
}

void WidgetButton::handleMouseButtonEvent(int button, float x, float y, bool pressed){
	if(listener != NULL && button == 1){
		if(pressed){
			this->pressed = true;
		}
		else{
			this->pressed = false;
			audio::playPlainSound("click", 0.6);
			listener->onButtonPressed(text);
		}

	}
}

void WidgetButton::render(float updateFactor){
	if(pressed){
		renderer.drawRect(0, 0, getWidth(), getHeight(), 0, colorButtonBackgroundPressed);
	}
	else{
		renderer.drawRect(0, 0, getWidth(), getHeight(), 0, colorButtonBackground);
	}
	renderer.drawString(0.05, getHeight() / 2, 0.05, text, colorButtonFont);
}

void WidgetButton::setListener(ButtonListener *l){
	listener = l;
}

} /* namespace gui */
