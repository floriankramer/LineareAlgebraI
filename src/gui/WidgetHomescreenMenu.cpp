/*
 * WidgetHomescreenMenu.cpp
 *
 *  Created on: Aug 26, 2015
 *      Author: dwarf
 */

#include "WidgetHomescreenMenu.h"
#include "CommandHandler.h"
#include "Game.h"
#include "Warp.h"

namespace gui {

WidgetHomescreenMenu::WidgetHomescreenMenu() : Widget(), buttonStart("Start"), buttonEditor("Editor"), buttonOptions("Options"), buttonExit("Exit") {

	this->setWidth(0.4);
	this->setHeight(0.7);
	setLocation(0, 0);


	buttonStart.setLocation(0, .30);
	buttonStart.setWidth(0.4);
	buttonStart.setHeight(.1);
	buttonStart.setListener(this);

	buttonEditor.setLocation(0, .10);
	buttonEditor.setWidth(0.4);
	buttonEditor.setHeight(.1);
	buttonEditor.setListener(this);

	buttonOptions.setLocation(0, -.10);
	buttonOptions.setWidth(0.4);
	buttonOptions.setHeight(.1);
	buttonOptions.setListener(this);

	buttonExit.setLocation(0, -.30);
	buttonExit.setWidth(0.4);
	buttonExit.setHeight(.1);
	buttonExit.setListener(this);

	addWidget(&buttonStart);
	addWidget(&buttonEditor);
	addWidget(&buttonOptions);
	addWidget(&buttonExit);

}

WidgetHomescreenMenu::~WidgetHomescreenMenu() {

}

void WidgetHomescreenMenu::render(float updateFactor){
	Widget::render(updateFactor);
}


void WidgetHomescreenMenu::onButtonPressed(std::string text){
	if(text == "Start"){
//		handleCommand("loadWorld test01");
//		handleCommand("setPaused false");
//		game::toGameWidget();
		game::getHomescreenWidget()->toWorldSelection();
	}
	else if(text == "Editor"){
		handleCommand("setEditMode");
		game::toGameWidget();
	}
	else if(text == "Options"){
		game::getHomescreenWidget()->toOptions();
	}
	else if(text == "Exit"){
		render::stop();
		game::stop();
	}
}

} /* namespace gui */
