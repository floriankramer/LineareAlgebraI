/*
 * Widget.cpp
 *
 *  Created on: Dec 22, 2014
 *      Author: dwarf
 */

#include "Widget.h"
#include "SDL2/SDL.h"
#include "GraphicsOptions.h"
#include "Warp.h"
#include "Game.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "WidgetConsole.h"
#include "CollisionHandler.h"
#include <algorithm>
#include <iostream>
#include <string>
#include "CoordinateConverter.h"
#include "AudioPlayer.h"

namespace gui {

Widget* focused = 0;
SDL_Event event;
Logger guiLogger("gui");
bool consoleActive = false;
WidgetConsole console;
bool editMode = false;

bool initializedJoysticks = false;

void setEditMode(bool b){
	editMode = b;
	game::getMasterWidget()->handleEditModeChange();
}
bool getEditMode(){
	return editMode;
}

Widget::Widget(): x(0), y(0), width(0), height(0), paddingX(0), paddingY(0), left(0), right(0), top(0), bottom(0), listenToKeys(false), listenToMouse(false), hasFocus(false), focusable(true){

}

Widget::~Widget() {
}

void Widget::handleInput(){

	if(!initializedJoysticks){
		SDL_Init(SDL_INIT_JOYSTICK);
		if(SDL_NumJoysticks() > 0){
			guiLogger.log("found joysticks: " + std::to_string(SDL_NumJoysticks()));
			SDL_JoystickEventState(SDL_ENABLE);
			SDL_JoystickOpen(0);
		}
		else{
			guiLogger.log("found no joystick. SDL Init stat: " + std::to_string(SDL_WasInit(SDL_INIT_JOYSTICK)));
		}
		initializedJoysticks = true;
	}

	while(SDL_PollEvent(&event)){
		float *s;
		std::vector<Widget*> *widgets;
		switch(event.type){

		case SDL_JOYAXISMOTION:
			if(focused != NULL){
				if(event.jaxis.value < -3200 || event.jaxis.value > 3200){
					focused->handleAxisEvent(event.jaxis.axis, event.jaxis.value / 32767.0);
				}
				else{
					focused->handleAxisEvent(event.jaxis.axis, 0);
				}
			}
			break;
		case SDL_JOYBUTTONDOWN:
			if(focused != NULL){
				focused->handleButtonEvent(event.jbutton.button, true);
			}
			break;
		case SDL_JOYBUTTONUP:
			if(focused != NULL){
				focused->handleButtonEvent(event.jbutton.button, false);
			}
			break;

		case SDL_MOUSEMOTION:

			s = pixelToScreenSpace(event.motion.x, event.motion.y);

			updateMousePosition(s[0], s[1]);

			widgets = new std::vector<Widget*>();
			getWidgetsAt(s[0], s[1], widgets);
			Widget *w;
			//if(getFocused() != NULL && getFocused()->isListeningToMouse()){
			while(!widgets->empty()){
				w = widgets->back();
				widgets->pop_back();
				if(w->isListeningToMouse()){
					w->handleMouseMotionEvent(s[0] - w->left, s[1] - w->bottom);
					for(Widget *w1 : w->children){
						if(!w1->isFocusable()){
							w1->handleMouseMotionEvent(s[0] - w1->left, s[1] - w1->bottom);
						}
					}
				}
			}
			//}

			delete widgets;
			delete[] s;
			break;
		case SDL_MOUSEBUTTONDOWN:
			updateMouseButton(event.button.button, true);


			s = pixelToScreenSpace(event.button.x, event.button.y);

			widgets = new std::vector<Widget*>();
			getWidgetsAt(s[0], s[1], widgets);
			if(consoleActive && game::physics::pointInRect(s[0], s[1], console.getLeft(), console.getBottom(), console.getWidth(), console.getHeight())){
				widgets->push_back(&console);
			}
			while(widgets->size() > 0){
				//Widget *w = widgets->at(i);
				Widget *w = widgets->back();
				widgets->pop_back();
				/**
				 * if the widget is listening to mouse events and focusable, let it handle the event, transforming the global event coordinates in local coordinates
				 * if the widget is also listening to keys, make it the currently focused widget
				 */
				if(w->isListeningToMouse() && w->isFocusable()){
					if(w->isListeningToKeys()){
						setFocus(w);
						//						if(focused != NULL)
						//							focused->hasFocus = false;
						//						focused = w;
						//						focused->hasFocus = true;
					}
					w->handleMouseButtonEvent(event.button.button, s[0] - w->left, s[1] - w->bottom, true);
					for(Widget *h : w->children){
						if(!h->isFocusable()){
							h->handleMouseButtonEvent(event.button.button, s[0] - h->left, s[1] - h->bottom, true);
						}
					}

					break;
				}
			}
			delete widgets;
			delete[] s;
			break;
		case SDL_MOUSEBUTTONUP:
			updateMouseButton(event.button.button, false);

			s = pixelToScreenSpace(event.button.x, event.button.y);

			//			x = (event.button.x * 2.0 / render::getDisplayWidth()) - 1;
			//			y = -((event.button.y * 2.0 / render::getDisplayHeight()) - 1);


			widgets = new std::vector<Widget*>();
			getWidgetsAt(s[0], s[1], widgets);
			if(consoleActive && game::physics::pointInRect(s[0], s[1], console.getLeft(), console.getBottom(), console.getWidth(), console.getHeight())){
				widgets->push_back(&console);
			}
			for(int i = widgets->size() - 1; i >=0; i--){
				Widget *w = widgets->at(i);
				/**
				 * if the widget is listening to mouse events, let it handle the event, transforming the global event coordinates in local coordinates
				 * if the widget is also listening to keys, make it the currently focused widget
				 */

				if(w->isListeningToMouse() && w->isFocusable()){
					float dX = 0;
					float dY = 0;
					w->handleMouseButtonEvent(event.button.button, s[0] - dX, s[1] - dY, false);
					break;
				}
			}
			delete widgets;
			delete[] s;
			break;
		case SDL_MOUSEWHEEL:
			if(getFocused() != NULL && getFocused()->isListeningToMouse()){
				getFocused()->handleMouseWheelEvent(event.wheel.y);
			}
			break;
		case SDL_KEYDOWN:
			updateKey(event.key.keysym.scancode, true);
			//mapKey(event.key.keysym.scancode, true);
			if(event.key.keysym.sym == SDLK_ESCAPE){
				if(game::isInHomescreen()){
					render::stop();
					game::stop();
				}
				else{
					game::toHomescreenWidget();
					audio::clearMusicQueue();
					audio::queueMusic("sfx/temp/pippin_the_hunchback.wav");
					audio::nextSong();
				}
			}
#ifdef unix
			if(event.key.keysym.sym == SDLK_BACKQUOTE && (event.key.keysym.mod & (KMOD_SHIFT | KMOD_CAPS)) > 0){
#else
#ifdef windows
				if(event.key.keysym.sym == SDLK_BACKQUOTE){ // TODO find way to get shift mod with wine SDL
#else
					if(false){
#endif
#endif
						consoleActive = !consoleActive;
						if(consoleActive){
							setFocus(&console);
						}
						else if(getFocused() == &console){
							if(children.size() > 0){
								setFocus(children[0]);
							}
							else{
								setFocus(NULL);
							}
						}
					}
					if(focused != NULL){
						focused->handleKeyEvent(event.key.keysym.sym, event.key.keysym.mod, true);
					}
					break;
		case SDL_TEXTINPUT:
			if(focused != NULL){
				focused->handleTextEvent(std::string(event.text.text));
			}
			break;
		case SDL_KEYUP:
			updateKey(event.key.keysym.scancode, false);
			//mapKey(event.key.keysym.scancode, false);
			if(focused != NULL){
				focused->handleKeyEvent(event.key.keysym.sym, event.key.keysym.mod, false);
			}
			break;
				}
			}

		}

		void Widget::render(float updateFactor){
			render::setRenderTarget(render::HUD);
			for(Widget *w : children){
				render::setScissorArea(w->left, w->bottom, w->width, w->height);
				w->render(updateFactor);
			}
			if(consoleActive){
				console.render(updateFactor);
			}
		}

		void Widget::handleMouseButtonEvent(int button, float x, float y, bool pressed){

		}

		void Widget::handleMouseMotionEvent(float x, float y){

		}

		void Widget::handleKeyEvent(int key, int mod, bool pressed){

		}

		void Widget::handleAxisEvent(int axis, float value){

		}

		void Widget::handleButtonEvent(int button, bool pressed){

		}

		void Widget::handleMouseWheelEvent(float value){

		}

		void Widget::handleTextEvent(std::string s){

		}

		void Widget::getWidgetsAt(float x, float y, std::vector<Widget*> *widgets){
			widgets->push_back(this);
			for(Widget *w : children){
				if(w->getLeft() <= x && w->getBottom() <= y && w->getRight() >= x && w->getTop() >= y){
					w->getWidgetsAt(x, y, widgets);
				}
			}
		}

		void Widget::setConsoleActive(bool a){
			consoleActive = a;
		}

		float Widget::getWidth(){
			if(width > 0){
				return width + paddingX;
			}
			else{
				float mW = 0;
				for(Widget *c : children){
					if(c->getX() + c->getWidth() > mW){
						mW = c->getX() + c->getWidth();
					}
				}
				return mW + paddingX;
			}
		}

		float Widget::getHeight(){
			if(height > 0){
				return height + paddingY;
			}
			else{
				float mH = 0;
				for(Widget *c : children){
					if(c->getX() + c->getHeight() > mH){
						mH = c->getY() + c->getHeight();
					}
				}
				return mH + paddingY;
			}
		}

		float Widget::getX(){
			return x;
		}

		float Widget::getY(){
			return y;
		}

		void Widget::setLocation(float x, float y){
			this->x = x;
			this->y = y;
			bottom = getY() - height / 2;
			top = getY() + height / 2;
			left = getX() - width / 2;
			right = getX() + width / 2;

			renderer.bottom = bottom + paddingY;
			renderer.top = top - paddingY;
			renderer.left = left - paddingX;
			renderer.right = right + paddingY;

		}

		void Widget::setWidth(float width){
			this->width = width;
			left = getX() - width / 2;
			right = getX() + width / 2;
			renderer.left = left - paddingX;
			renderer.right = right + paddingY;
		}

		float Widget::getPaddingX() {
			return paddingX;
		}

		void Widget::setPaddingX(float paddingX) {
			this->paddingX = paddingX;
			renderer.left = left - paddingX;
			renderer.right = right + paddingY;
		}

		float Widget::getPaddingY() {
			return paddingY;
		}

		void Widget::setPaddingY(float paddingY) {
			this->paddingY = paddingY;
			renderer.bottom = bottom + paddingY;
			renderer.top = top - paddingY;
		}

		void Widget::setHeight(float height) {
			this->height = height;
			bottom = getY() - height / 2;
			top = getY() + height / 2;
			renderer.bottom = bottom + paddingY;
			renderer.top = top - paddingY;
		}

		bool Widget::isListeningToKeys() {
			return listenToKeys;
		}

		void Widget::setListenToKeys(bool listenToKeys) {
			this->listenToKeys = listenToKeys;
		}

		bool Widget::isListeningToMouse() {
			return listenToMouse;
		}

		void Widget::setListenToMouse(bool listenToMouse) {
			this->listenToMouse = listenToMouse;
		}

		void Widget::onAdd(Widget *w){

		}

		void Widget::onRemove(Widget *w){
			w->doRemove(this);
		}

		void Widget::addWidget(Widget *w){
			children.push_back(w);
			w->getParents()->push_back(this);
			w->onAdd(this);
		}

		void Widget::doRemove(Widget *w){
			for(unsigned int i = 0; i < children.size(); i++){
				if(children.at(i) == w){
					children.erase(children.begin() + i);
					return;
				}
			}
			for(unsigned int i = 0; i < w->getParents()->size(); i++){
				if(w->getParents()->at(i) == this){
					w->getParents()->erase(w->getParents()->begin() + i);
					return;
				}
			}
		}

		void Widget::removeWidget(Widget *w){
			//children.erase(std::remove(children.begin(), children.end(), w), children.end());
			//	for(unsigned int i = 0; i < children.size(); i++){
			//		if(children.at(i) == w){
			//			children.erase(children.begin() + i);
			//			return;
			//		}
			//	}
			//	for(unsigned int i = 0; i < w->getParents()->size(); i++){
			//		if(w->getParents()->at(i) == this){
			//			w->getParents()->erase(w->getParents()->begin() + i);
			//			return;
			//		}
			//	}

			w->onRemove(this);
		}

		bool Widget::hasChild(Widget *w){
			for(Widget *e: children){
				if(w == e){
					return true;
				}
			}
			return false;
		}

		float Widget::getLeft(){
			return left;
		}
		float Widget::getRight(){
			return right;
		}
		float Widget::getTop(){
			return top;
		}
		float Widget::getBottom(){
			return bottom;
		}

		void Widget::setHasFocus(bool focused){
			this->hasFocus = focused;
		}

		bool Widget::isFocused(){
			return hasFocus;
		}

		std::vector<Widget*>* Widget::getParents(){
			return &parents;
		}

		std::vector<Widget*>* Widget::getChildren(){
			return &children;
		}

		bool Widget::isFocusable(){
			return focusable;
		}

		void Widget::setFocusable(bool f){
			focusable = f;
		}

		void Widget::handleEditModeChange(){
			this->onEditModeChanged(editMode);
			for(Widget *w : children){
				w->handleEditModeChange();
			}
		}
		void Widget::onFocusChanged(){}
		void Widget::onEditModeChanged(bool b){}


		void setFocus(Widget *w){
			if(focused != w){
				if(focused != NULL){
					focused->setHasFocus(false);
					focused->onFocusChanged();
				}
				focused = w;
				if(focused != NULL){
					focused->setHasFocus(true);
					focused->onFocusChanged();
				}
			}
		}

		Widget *getFocused(){
			return focused;
		}


		bool getConsoleActive(){
			return consoleActive;
		}




	} /* namespace game */
