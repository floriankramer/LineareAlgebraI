/*
 * WidgetButton.cpp
 *
 *  Created on: Mar 22, 2015
 *      Author: dwarf
 */

#include <WidgetButton.h>
#include "Renderer.h"
#include "AudioPlayer.h"


namespace warp{
namespace gui {

warp::Color colorButtonFont(0, 0, 0);
warp::Color colorButtonBackground(0.7, 0.7, 0.7);
warp::Color colorButtonBackgroundPressed(0.6, .6, .6);

WidgetButton::WidgetButton(std::string text) : Widget(){
	listener = 0;
	this->text = text;
	setListenToMouse(true);
	pressed = false;
	setWidth(6);
	setHeight(1);

	name = "WidgetButton";
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
		renderer->drawRect(0, 0, getWidth(), getHeight(), 0, colorButtonBackgroundPressed);
	}
	else{
		renderer->drawRect(0, 0, getWidth(), getHeight(), 0, colorButtonBackground);
	}
	renderer->drawString(0.5, 0, 0.9, text, colorButtonFont);
}

void WidgetButton::setListener(ButtonListener *l){
	listener = l;
}

} /* namespace warp{
namespace gui */
}
