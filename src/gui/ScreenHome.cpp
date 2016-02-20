/*
 * WidgetHomescreen.cpp
 *
 *  Created on: Mar 22, 2015
 *      Author: dwarf
 */

#include <ScreenHome.h>
#include "Renderer.h"
#include "CommandHandler.h"
#include "Game.h"
#include "GraphicsOptions.h"

namespace gui {

WidgetHomescreen::WidgetHomescreen() : Widget(), homescreenMenu(), worldSelection(), options() {
	this->setWidth(20 * render::options::getAspectRatio());
	this->setHeight(20);
	setLocation(0, 0);
	this->addWidget(&homescreenMenu);
	homescreenMenu.setLocation(getWidth() / 2 - homescreenMenu.getWidth() / 2, 5);
	worldSelection.setLocation(getWidth() / 2 - homescreenMenu.getWidth() / 2, 5);
	options.setLocation(1, 1);
	options.setWidth(getWidth() - 2);
	options.setHeight(getHeight() - 2);
}

WidgetHomescreen::~WidgetHomescreen() {

}

void WidgetHomescreen::render(float updateFactor){
	if(render::options::getVirtualWidth() / render::options::getVirtualHeight() > 1.777778){
		renderer->drawSprite(0, 0, getWidth(), getWidth() * 0.5625, 0, std::string("map"));
	}
	else{
		renderer->drawSprite(0, 0, getWidth(), getHeight(), 0, std::string("map"));
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
