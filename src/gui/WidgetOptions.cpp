/*
 * WidgetOptions.cpp
 *
 *  Created on: Aug 26, 2015
 *      Author: dwarf
 */

#include <WidgetOptions.h>
#include "GraphicsOptions.h"
#include "Game.h"
#include "CommandHandler.h"
#include "Renderer.h"
#include "AudioPlayer.h"
#include "OptionsHandler.h"
namespace gui {

render::Color optionsBackgroundColor(0.5, 0.5, 0.5, 0.7);

WidgetOptions::WidgetOptions() : Widget(), buttonBack("cancel"), buttonApply("apply"), toggleFullscreen("fullscreen", false), textResolution() {
	this->setWidth(1.8 * render::options::getAspectRatio());
	this->setHeight(1.8);
	setLocation(0, 0);

	buttonBack.setLocation(0.5, -0.8);
	buttonBack.setListener(this);

	buttonApply.setLocation(-0.5, -0.8);
	buttonApply.setListener(this);

	toggleFullscreen.setLocation(-.3, .5);
	toggleFullscreen.setListener(this);

	textResolution.setLocation(-.2, .35);
	textResolution.setWidth(.4);
	textResolution.setHeight(.1);

	masterVolume.setLocation(0, .1);
	musicVolume.setLocation(0, -.1);

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
	render::drawRect(0, 0, getWidth(), getHeight(), 0, optionsBackgroundColor);

	render::drawString(-.9, .7, .07, "Graphics:");
	render::drawString(-.9, .5, .07, "Fullscreen:");
	render::drawString(-.9, .35, .07, "Resolution:");
	render::drawString(-.9, .2, .07, "Audio:");
	render::drawString(-.9, .1, .07, "Master Volume:");
	render::drawString(-.9, -.1, .07, "Music Volume:");


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
