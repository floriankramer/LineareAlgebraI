/*
 * NodeValue.cpp
 *
 *  Created on: Mar 11, 2015
 *      Author: dwarf
 */

#include <NodeValue.h>
#include <stdexcept>
#include "Renderer.h"
#include "Game.h"
#include "CoordinateConverter.h"

namespace game {

render::Color colorValueBackground(0.7, 0.7, 0.7);

NodeValue::NodeValue() {
	addOutput(0, INT);
	addOutput(1, BOOL);
	addOutput(2, FLOAT);
	textField.setWidth(0.3);
	textField.setHeight(0.06);
	textField.setTextFieldListener(this);
	addedTextField = false;
	setType(7);
}

NodeValue::~NodeValue() {
	if(addedTextField){
		getWidgetWorld()->removeWidget(&textField);
	}
}

void NodeValue::render(bool editMode, float updateFactor ){
	if(editMode){
		render::drawRect(getXCenter(), getYCenter(), getWidth(), getHeight(), 0, colorValueBackground);

		float *f = worldToScreenSpace(getXCenter(), getYCenter());
		textField.setLocation(f[0], f[1]);
//		textField.setWidth(getWidth() - 0.1);
//		textField.setHeight(getHeight() - 0.1);
		if(!addedTextField){
			getWidgetWorld()->addWidget(&textField);
			addedTextField = true;
		}

		delete[] f;
	}
	else{
		if(addedTextField){
			getWidgetWorld()->removeWidget(&textField);
			addedTextField = false;
		}
	}
}

void NodeValue::onTextChanged(std::string text){
	if(text == "true"){
		setHandleData(0, HandleData(true));
		setHandleData(1, HandleData(true));
		setHandleData(2, HandleData(true));
	}
	else if(text == "false"){
		setHandleData(0, HandleData(false));
		setHandleData(1, HandleData(false));
		setHandleData(2, HandleData(false));
	}
	else{
		if(text.find(".") == text.npos){
			try{
				int i = std::stoi(text);
				setHandleData(0, HandleData(i));
				setHandleData(1, HandleData(i));
				setHandleData(2, HandleData(i));
			}
			catch(std::invalid_argument &e){

			}
		}
		else{
			try{
				float f = std::stof(text);
				setHandleData(0, HandleData(f));
				setHandleData(1, HandleData(f));
				setHandleData(2, HandleData(f));
			}
			catch(std::invalid_argument &e2){

			}
		}
	}
}

} /* namespace game */
