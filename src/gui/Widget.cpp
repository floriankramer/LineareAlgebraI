/*
 * Widget.cpp
 *
 *  Created on: Dec 22, 2014
 *      Author: dwarf
 */

#include "WidgetConsole.h"
#include "Widget.h"
#include "SDL2/SDL.h"
#include "GraphicsOptions.h"
#include "Warp.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "CollisionHandler.h"
#include <algorithm>
#include <iostream>

#include "CoordinateConverter.h"
#include "AudioPlayer.h"

namespace warp{
namespace gui {

Widget* focused = 0;
SDL_Event event;
warp::Logger guiLogger("gui");
bool consoleActive = false;
WidgetConsole console;
bool editMode = false;

bool initializedJoysticks = false;

void setEditMode(bool b){
	editMode = b;
	//game::getMasterWidget()->handleEditModeChange();
}
bool getEditMode(){
	return editMode;
}

Widget::Widget(): name("Widget"),  x(0), y(0), width(0), height(0), paddingLeft(0), paddingTop(0), paddingBottom(0), paddingRight(0), parent(NULL), listenToKeys(false), listenToMouse(false), hasFocus(false), focusable(true), scissorLeft(0), scissorBottom(0), scissorWidth(0), scissorHeight(0){
	renderer = new WidgetRenderer(this);
}

Widget::~Widget() {
	delete renderer;
}

void Widget::handleInput(){

	if(!initializedJoysticks){
		SDL_Init(SDL_INIT_JOYSTICK);
		if(SDL_NumJoysticks() > 0){
			guiLogger.log("found joysticks: %d" , SDL_NumJoysticks());
			SDL_JoystickEventState(SDL_ENABLE);
			SDL_JoystickOpen(0);
		}
		else{
			guiLogger.log("found no joystick. SDL Init stat: %d", SDL_WasInit(SDL_INIT_JOYSTICK));
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
					w->handleMouseMotionEvent(s[0] - w->getAbsoluteLeft(), s[1] - w->getAbsoluteTop());
					for(Widget *w1 : w->children){
						if(!w1->isFocusable()){
							w1->handleMouseMotionEvent(s[0] - w1->getAbsoluteLeft(), s[1] - w1->getAbsoluteTop());
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
			if(consoleActive && warp::pointInRect(s[0], s[1], console.getAbsoluteLeft(), console.getAbsoluteTop(), console.getWidth(), console.getHeight())){
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
					w->handleMouseButtonEvent(event.button.button, s[0] - w->getAbsoluteLeft(), s[1] - w->getAbsoluteTop(), true);
					for(Widget *h : w->children){
						if(!h->isFocusable()){
							h->handleMouseButtonEvent(event.button.button, s[0] - h->getAbsoluteLeft(), s[1] - h->getAbsoluteTop(), true);
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

			widgets = new std::vector<Widget*>();
			getWidgetsAt(s[0], s[1], widgets);
			if(consoleActive && warp::pointInRect(s[0], s[1], console.getAbsoluteLeft(), console.getAbsoluteTop(), console.getWidth(), console.getHeight())){
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
					w->handleMouseButtonEvent(event.button.button, s[0] - w->getAbsoluteLeft(), s[1] - w->getAbsoluteTop(), false);
					for(Widget *h : w->children){
						if(!h->isFocusable()){
							h->handleMouseButtonEvent(event.button.button, s[0] - h->getAbsoluteLeft(), s[1] - h->getAbsoluteTop(), false);
						}
					}

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
				/*if(game::isInHomescreen()){
					warp::stop();
					game::stop();
				}
				else{
					game::toHomescreenWidget();
					audio::clearMusicQueue();
					audio::queueMusic("sfx/temp/pippin_the_hunchback.wav");
					audio::nextSong();
				}*/
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
			warp::setRenderTarget(warp::HUD); //TODO unnecessary repeated setting of render Target
			for(Widget *w : children){
				//warp::setScissorArea(w->getAbsoluteLeft(), w->getAbsoluteTop() + w->getHeight(), w->getWidth(), w->getHeight());
				warp::setScissorArea(scissorLeft, scissorBottom, scissorWidth, scissorHeight);
				w->render(updateFactor);
			}
		}

		void Widget::renderConsole(float updateFactor){
			warp::setRenderTarget(warp::HUD);
			if(consoleActive){//TODO think about using new scissoring variables for the console (curently thei are out of scope)
				warp::setScissorArea(console.getAbsoluteLeft(), console.getAbsoluteTop() + console.getHeight(), console.getWidth(), console.getHeight());
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
			x -= getLeft() + paddingLeft;
			y -= getTop() + paddingTop;
			for(Widget *w : children){
				if(w->getLeft() <= x && w->getTop() <= y && w->getRight() >= x && w->getBottom() >= y){
					w->getWidgetsAt(x, y, widgets);
				}
			}
		}

		void Widget::setConsoleActive(bool a){
			consoleActive = a;
		}

		float Widget::getWidth(){
			//			if(width > 0){
			return width;// + paddingLeft + paddingRight;
			//			}
			//			else{ TODO overthink the need of this. Shouldn't be necessary in a well built ui
			//				float mW = 0;
			//				for(Widget *c : children){
			//					if(c->getX() + c->getWidth() > mW){
			//						mW = c->getX() + c->getWidth();
			//					}
			//				}
			//				return mW + paddingX;
			//			}
		}

		float Widget::getHeight(){
			//			if(height > 0){
			return height;// + paddingLeft + paddingRight;
			//			}
			//			else{
			//				float mH = 0;
			//				for(Widget *c : children){
			//					if(c->getX() + c->getHeight() > mH){
			//						mH = c->getY() + c->getHeight();
			//					}
			//				}
			//				return mH + paddingY;
			//			}
		}

		void Widget::setLocation(float x, float y){
			this->x = x;
			this->y = y;
			updateScissorVariables();
		}

		void Widget::setWidth(float width){
			this->width = width;
			updateScissorVariables();
		}

		void Widget::setHeight(float height) {
			this->height = height;
			updateScissorVariables();
		}

		void Widget::setPaddingX(float f) {
			paddingLeft = paddingRight = f;
			updateScissorVariables();
		}

		void Widget::setPaddingY(float f) {
			paddingTop = paddingBottom = f;
			updateScissorVariables();
		}

		void Widget::setPaddingLeft(float f){
			paddingLeft = f;
			updateScissorVariables();
		}

		void Widget::setPaddingTop(float f){
			paddingTop = f;
			updateScissorVariables();
		}

		void Widget::setPaddingBottom(float f){
			paddingBottom = f;
			updateScissorVariables();
		}

		void Widget::setPaddingRight(float f){
			paddingRight = f;
			updateScissorVariables();
		}

		float Widget::getPaddingLeft(){
			return paddingLeft;
		}


		float Widget::getPaddingTop(){
			return paddingTop;
		}


		float Widget::getPaddingBottom(){
			return paddingBottom;
		}


		float Widget::getPaddingRight(){
			return paddingRight;
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
			w->setParent(this);
			w->onAdd(this);
			w->updateScissorVariables();
		}

		void Widget::doRemove(Widget *w){
			children.erase(remove(children.begin(), children.end(), w), children.end());
			w->setParent(NULL);
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

		void Widget::setHasFocus(bool focused){
			this->hasFocus = focused;
		}

		bool Widget::isFocused(){
			return hasFocus;
		}

		Widget* Widget::getParent(){
			return parent;
		}

		void Widget::setParent(Widget *w){
			parent = w;
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

		float Widget::getRight(){
			return x + width;
		}

		float Widget::getBottom(){
			return y + height;
		}

		float Widget::getCenterX(){
			return x + width / 2;
		}

		float Widget::getCenterY(){
			return y + height / 2;
		}

		float Widget::getLeft(){
			return x;
		}

		float Widget::getTop(){
			return y;
		}

		/**
		 * @return returns the position of the left edge in relation to the root widget
		 */
		float Widget::getAbsoluteLeft(){
			if(parent != NULL){
				return parent->getAbsoluteLeft() + x + parent->paddingLeft;	
			}
			else{
				return x;
			}
		}

		/**
		 * @return returns the position of the top edge in relation to the root widget
		 */
		float Widget::getAbsoluteTop(){
			if(parent != NULL){	
				return parent->getAbsoluteTop() + parent->paddingTop + y;
			}
			else{
				return y;
			}
		}

		float Widget::getAbsoluteBottom(){
			return getAbsoluteTop() + getHeight();
		}

		float Widget::getAbsoluteRight(){
			return getAbsoluteLeft() + getWidth();
		}

		void Widget::updateScissorVariables(){
			if(parent != NULL){
				float pMinLeft = parent->getAbsoluteLeft() + parent->getPaddingLeft();
				if(pMinLeft < parent->scissorLeft){
					pMinLeft = parent->scissorLeft;
				}
				float pMinBottom = parent->getAbsoluteBottom() - parent->getPaddingBottom();
				if(pMinBottom > parent->scissorBottom){
					pMinBottom = parent->scissorBottom;
				}
				float pMaxRight = parent->getAbsoluteRight() - parent->getPaddingRight();
				if(pMaxRight > parent->scissorLeft + parent->scissorWidth){
					pMaxRight = parent->scissorLeft + parent->scissorWidth;
				}
				float pMaxTop = parent->getAbsoluteTop() + parent->getPaddingTop();
				if(pMaxTop < parent->scissorBottom + parent->scissorHeight){
					pMaxTop = parent->scissorBottom - parent->scissorHeight;
				}
				float absLeft = getAbsoluteLeft();
				float absTop = getAbsoluteTop();
				float absRight = getAbsoluteRight();
				float absBottom = getAbsoluteBottom();
				
				scissorLeft = pMinLeft > absLeft ? pMinLeft : absLeft;
				scissorBottom = pMinBottom > absBottom ? pMinBottom : absBottom;
				scissorWidth = (pMaxRight > absRight ? absRight : pMaxRight) - scissorLeft; 
				scissorHeight = scissorBottom - (pMaxTop > absTop ? absTop : pMaxTop);
			}
			else{
				scissorLeft = getAbsoluteLeft();
				scissorBottom = getAbsoluteBottom();
				scissorWidth = getWidth();
				scissorHeight = getHeight();
			}
			for(Widget *c : children){
				c->updateScissorVariables();
			}
		}


		//Definition of the Renderer Helper class. Might pu this into its own files

		WidgetRenderer::WidgetRenderer(Widget *owner): owner(owner){
		}

		WidgetRenderer::~WidgetRenderer(){

		}

		void WidgetRenderer::drawSprite(float x, float y, float width, float height, float rotation, std::string texture){
			//warp::drawSprite(x + left + width / 2, y + bottom + height / 2, width, height, rotation, texture);
			warp::drawSprite(owner->getAbsoluteLeft() + x + width / 2, owner->getAbsoluteTop() + y + height / 2, width, -height, rotation, texture);
		}

		void WidgetRenderer::drawSpriteCentered(float x, float y, float width, float height, float rotation, std::string texture){
			warp::drawSprite(owner->getAbsoluteLeft() + x, owner->getAbsoluteTop() + y, width, -height, rotation, texture);
		}

		void WidgetRenderer::drawLine(float x, float y, float x2, float y2, float thickness, warp::Color c ){
			warp::drawLine(owner->getAbsoluteLeft() + x, owner->getAbsoluteTop() + y, owner->getAbsoluteLeft() + x2,  owner->getAbsoluteTop() + y2, thickness, c);
		}

		void WidgetRenderer::drawRect(float x, float y, float width, float height, float rot, warp::Color c ){
			warp::drawRect(owner->getAbsoluteLeft() + x + width / 2, owner->getAbsoluteTop() + y + height / 2, width, -height, rot, c);
		}

		void WidgetRenderer::drawCurve(float x1, float y1, float x2, float y2, float dx1, float dy1, float dx2, float dy2, float thickness, warp::Color c){
			warp::drawCurve(owner->getAbsoluteLeft() + x1, owner->getAbsoluteTop() + y1, owner->getAbsoluteLeft() + x2, owner->getAbsoluteTop() + y2, dx1, dy1, dx2, dy2, thickness, c);
		}

		void WidgetRenderer::drawRectOutline(float x, float y, float width, float height, float rot, float thickness, warp::Color c){
			warp::drawRectOutline(owner->getAbsoluteLeft() + x + width / 2, owner->getAbsoluteTop() + y + height / 2, width, -height, rot, thickness, c);
		}

		void WidgetRenderer::drawString(float x, float y, float lineheight, std::string string, warp::Color c){
			warp::drawString(owner->getAbsoluteLeft() + x, owner->getAbsoluteTop() + y + lineheight / 2, lineheight, string, c);
		}


	} /* namespace warp{
namespace gui */
}
