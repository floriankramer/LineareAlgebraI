/*
 * WidgetHomescreen.cpp
 *
 *  Created on: Mar 22, 2015
 *      Author: dwarf
 */

#include <WidgetHomescreen.h>
#include "Renderer.h"
#include "CommandHandler.h"
#include "Game.h"
#include "GraphicsOptions.h"

namespace gui {

WidgetHomescreen::WidgetHomescreen() : Widget(), homescreenMenu(), worldSelection(), options() {
	this->setWidth(2 * render::options::getAspectRatio());
	this->setHeight(2);
	setLocation(0, 0);
	this->addWidget(&homescreenMenu);
}

WidgetHomescreen::~WidgetHomescreen() {

}

void WidgetHomescreen::render(float updateFactor){
	if(render::options::getVirtualWidth() / render::options::getVirtualHeight() > 1.777778){
		render::drawSprite(0, 0, 2 * render::options::getAspectRatio(), 2 * render::options::getAspectRatio() * 0.5625, 0, std::string("map"));
	}
	else{
		render::drawSprite(0, 0, 1.77777 * 2, 2, 0, std::string("map"));
	}

	Widget::render(updateFactor);
}

void WidgetHomescreen::toMainMenu(){
	this->removeWidget(&worldSelection);
	this->removeWidget(&options);
	this->addWidget(&homescreenMenu);
}

void WidgetHomescreen::toWorldSelection(){
	this->removeWidget(&homescreenMenu);
	this->removeWidget(&options);
	this->addWidget(&worldSelection);
}

void WidgetHomescreen::toOptions(){
	this->removeWidget(&homescreenMenu);
	this->removeWidget(&worldSelection);
	this->addWidget(&options);
	options.updateValues();
}

} /* namespace gui */
