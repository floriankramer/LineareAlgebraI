/*
 * Tool.cpp
 *
 *  Created on: Feb 6, 2015
 *      Author: dwarf
 */

#include <Tool.h>

namespace gui {

render::Color toolOverlayColor(0, 0, 0.7, 0.3);

Tool::Tool() {

}

Tool::~Tool() {
}

void Tool::begin(float, float, int){

}

void Tool::step(float, float, int){

}

void Tool::stop(float, float, int){

}

void Tool::onKeyPress(int button, int modifier, bool pressed, Widget *parent){

}

std::string Tool::setParameter(std::string name, std::string value){
	return "";
}

std::string Tool::getName(){
	return "Tool";
}

void Tool::render(float updateFactor){

}

} /* namespace gui */
