/*
 * WidgetTextField.cpp
 *
 *  Created on: Mar 11, 2015
 *      Author: dwarf
 */

#include <WidgetTextField.h>
#include "Renderer.h"
#include "SDL2/SDL_keycode.h"
#include <map>
#include "TextHelper.h"

namespace gui {


render::Color colorTextBackground(0, 0, 0);
render::Color colorTextFont;

WidgetTextField::WidgetTextField() : Widget(){
	setListenToKeys(true);
	setListenToMouse(true);
	cursorPos = 0;
	blinkState = 0;
	listener = 0;
	text = "";
}

WidgetTextField::WidgetTextField(float width, float height) {
	setListenToKeys(true);
	setListenToMouse(true);
	setWidth(width);
	setHeight(height);
	cursorPos = 0;
	blinkState = 0;
	listener = 0;
	text = "";
}

WidgetTextField::~WidgetTextField() {

}

void WidgetTextField::render(float updateFactor){
	if(isFocused()){
		blinkState = (blinkState + 1) % 60;
	}
	renderer->drawRect(0, 0, getWidth(), getHeight(), 0, colorTextBackground);
	//	renderer->drawString(0.05, getHeight() / 2, 0.05, text, colorTextFont);
	if(blinkState / 30 == 1 && isFocused()){
		renderer->drawString(0.1, 0, 0.9 * getHeight(), text.substr(0, cursorPos) + "_", colorTextFont);
	}
	renderer->drawString(0.1, 0, 0.9 * getHeight(), text, colorTextFont);
}

void WidgetTextField::handleKeyEvent(int k, int mod,  bool pressed){
	if(pressed){
		if(k == SDLK_BACKSPACE ){
			if(cursorPos > 0){
				text.erase(cursorPos - 1, 1);
				cursorPos --;
			}
		}
		else if(k == SDLK_DELETE){
			if(cursorPos < text.length()){
				text.erase(cursorPos, 1);
			}
		}
		else if(k == SDLK_LEFT){
			if(cursorPos > 0){
				cursorPos --;
			}
		}
		else if(k == SDLK_RIGHT){
			if(cursorPos < text.length()){
				cursorPos ++;
			}
		}
		else if(k == SDLK_BACKQUOTE && (mod & (KMOD_SHIFT | KMOD_CAPS)) > 0){

		}
		else if(k != SDLK_LSHIFT && k != SDLK_RSHIFT && k != SDLK_CAPSLOCK){
			if(mod & (KMOD_SHIFT | KMOD_CAPS)){
				if(k >= 97 && k <= 123){
					text.insert(cursorPos, std::string(1, (char)(k - 32)));
					cursorPos ++;
				}
				else{
					text.insert(cursorPos, std::string(1, shiftMap[(char)k]));
					cursorPos ++;
				}
			}
			else{
				text.insert(cursorPos, std::string(1, (char)k));
				cursorPos ++;
			}
		}

		if(listener != NULL){
			listener->onTextChanged(text);
		}
	}


}

void WidgetTextField::setText(std::string t){
	text = t;
}

std::string WidgetTextField::getText(){
	return text;
}

void WidgetTextField::setTextFieldListener(TextFieldListener *l){
	listener = l;
}
}
/* namespace game */
