/*
 * WidgetOptions.cpp
 *
 *  Created on: Aug 26, 2015
 *      Author: dwarf
 */

#include <ScreenOptions.h>
#include "GraphicsOptions.h"
#include "Game.h"
#include "CommandHandler.h"
#include "Renderer.h"
#include "AudioPlayer.h"
#include "OptionsHandler.h"
namespace gui {

render::Color optionsBackgroundColor(0.5, 0.5, 0.5, 0.7);

const unsigned int optElemOffset = 7;

WidgetOptions::WidgetOptions() : Widget(), buttonBack("cancel"), buttonApply("apply"), toggleFullscreen("fullscreen", false), textResolution() {
	buttonBack.setLocation(1, 16);
	buttonBack.setListener(this);

	buttonApply.setLocation(8, 16);
	buttonApply.setListener(this);

	toggleFullscreen.setLocation(optElemOffset, 1);
	toggleFullscreen.setListener(this);

	textResolution.setLocation(optElemOffset, 2);
	textResolution.setWidth(4);
	textResolution.setHeight(1);

	masterVolume.setLocation(optElemOffset, 4);
	musicVolume.setLocation(optElemOffset, 5);

	addWidget(&buttonBack);
	addWidget(&buttonApply);
	addWidget(&toggleFullscreen);
	addWidget(&textResolution);
	addWidget(&masterVolume);
	addWidget(&musicVolume);

}

WidgetOptions::~WidgetOptions() {

}

void WidgetOptions::updateValues(){
	toggleFullscreen.setState(render::options::isFullscreen());
	textResolution.setText(std::to_string(render::options::getFBOWidth()) + " " + std::to_string(render::options::getFBOHeight()));
	masterVolume.setValue(audio::getGlobalVolume());
	musicVolume.setValue(audio::getMusicVolume());
}

void WidgetOptions::onButtonPressed(std::string text){
	if(text == "apply"){
		applyChanges();
		game::getHomescreenWidget()->toMainMenu();
	}
	else if(text == "cancel"){
		game::getHomescreenWidget()->toMainMenu();
	}
}

void WidgetOptions::onToggled(std::string name, bool state){
	if(name == "fullscreen"){
		gui::handleCommand(std::string("setFullscreen ") + (state ? "true" : "false"));
	}
}

void WidgetOptions::render(float updateFactor){
	renderer->drawRect(0, 0, getWidth(), getHeight(), 0, optionsBackgroundColor);

	renderer->drawString(0.3, 0, .7, "Graphics:");
	renderer->drawString(0.7, 1, .7, "Fullscreen:");
	renderer->drawString(0.7, 2, .7, "Resolution:");
	renderer->drawString(0.3, 3, .7, "Audio:");
	renderer->drawString(0.7, 4, .7, "Master Volume:");
	renderer->drawString(0.7, 5, .7, "Music Volume:");


	Widget::render(updateFactor);
}

void WidgetOptions::applyChanges(){
	std::string res = textResolution.getText();
	unsigned int i = res.find(" ");
	if(i != res.npos){
		render::options::setResolution(std::stoi(res.substr(0, i)), std::stoi(res.substr(i, res.size())));
	}
//	render::options::saveOptions();
	audio::setGlobalVolume(masterVolume.getValue());
	audio::setMusicVolume(musicVolume.getValue());
	saveOptions();

		//
	}

} /* namespace gui */
