/*
 * WidgetRadialMenu.cpp
 *
 *  Created on: Feb 15, 2015
 *      Author: dwarf
 */

#include "WidgetRadialMenu.h"
#include <cmath>
#include "Renderer.h"



namespace gui {

const float subAngle = 3.1415;

render::Color colorLine(0.3, 0.3, 0.8, 0.7);
render::Color colorText(0.7, 0.7, 0.7, 0.7);
render::Color colorTextHighlight;

WidgetRadialMenu::WidgetRadialMenu() : Widget(){
	radius = 0.2;
	this->setWidth(radius * 2 + 0.05);
	this->setHeight(radius * 2 + 0.05);
	this->setListenToMouse(true);
	this->setListenToKeys(true);
	this->setLocation(0, 0);
	setFocusable(false);
	callbackFunction = NULL;

	numOptions = 0;
	selected = 0;
	options.sliceSize = 0;
	callbackFunctionData = NULL;
}

WidgetRadialMenu::~WidgetRadialMenu() {
}

void WidgetRadialMenu::onAdd(Widget *w){
	//	animState = 1;
	//	animValue = 0.05;
	options.open();
}

void WidgetRadialMenu::onRemove(Widget *w){
	//	animState = 2;
	for(int i = 0; i < options.getShallowSize(); i++){
		options.children[i].close();
	}
	options.close();
}



void WidgetRadialMenu::render(float updateFactor){

	//	if(animState == 1){
	//		animValue += updateFactor * 3;//(updateFactor / 0.05) * animValue * (1.05 - animValue);
	//		if(animValue >= 1){
	//			animValue = 1;
	//			animState = 0;
	//		}
	//		posFactor = sin(animValue * 3.1415 / 2);
	//	}
	//	else if(animState == 2){
	//		animValue -= updateFactor * 4;
	//		if(animValue <= 0){
	//			getParents()->at(0)->doRemove(this); //TODO think about adding to multiple widgets at once
	//		}
	//		posFactor = sin(animValue * 3.1415 / 2);
	//	}

	options.updateAnim(updateFactor);

	if(options.posFactor == 0){
		getParents()->at(0)->doRemove(this); //TODO think about adding to multiple widgets at once
	}

	if(options.getShallowSize() > 0){

		float x = getX(), y = getY();
		//		options.x = x;
		//		options.y = y;

		for(unsigned int i = 0; i < options.children.size(); i++){
			renderTree.push_back(&options.children[i]);
		}

		while(renderTree.size() > 0){
			RadialMenuTree *t = renderTree.back();
			renderTree.pop_back();

			if(selected == t || selected->parent == t){
				if(t->getShallowSize() == 0 || selectionDepth == 0){
					render::drawLine(x + t->parent->x , y + t->parent->y, x + t->parent->x + t->x * 0.8 * t->parent->posFactor, y + t->parent->y + t->y * 0.8 * t->parent->posFactor, 0.005, colorLine);
					if(t->textureName.length() > 0){
						render::drawSprite(x + t->parent->x + t->x * t->parent->posFactor, y + t->parent->y + t->y * t->parent->posFactor, t->textureWidth * 1.1, t->textureHeight * 1.1, 0, t->textureName);
					}
					else
						render::drawString(x + t->parent->x + t->x * t->parent->posFactor, y + t->parent->y + t->y * t->parent->posFactor, 0.05, t->name, colorTextHighlight);

				}
				else{
					render::drawLine(x + t->parent->x , y + t->parent->y, x + t->parent->x + t->x * t->parent->posFactor, y + t->parent->y + t->y * t->parent->posFactor, 0.005, colorLine);
				}
				//				if(selected->parent == t){
				if(t->posFactor > 0){
					for(unsigned int i = 0; i < t->children.size(); i++){
						renderTree.push_back(&(t->children[i]));
					}
				}
				//				}
			}
			else{
				render::drawLine(x + t->parent->x , y + t->parent->y, x + t->parent->x + t->x * 0.8 * t->parent->posFactor, y + t->parent->y + t->y * 0.8 * t->parent->posFactor, 0.005, colorLine);
				if(t->textureName.length() > 0){
					render::drawSprite(x + t->parent->x + t->x * t->parent->posFactor, y + t->parent->y + t->y * t->parent->posFactor, t->textureWidth, t->textureHeight, 0, t->textureName);
				}
				else
					render::drawString(x + t->parent->x + t->x * t->parent->posFactor, y + t->parent->y + t->y * t->parent->posFactor, 0.05, t->name, colorTextHighlight);

			}



		}

		//		float x = getX(), y = getY();
		//		for(int i = 0; i < numOptions; i++){
		//			render::drawLine(x, y, x + positions[i * 2] * 0.8 * posFactor, y + positions[i * 2 + 1] * 0.8 * posFactor, 0.005, colorLine);
		//			if(selected == i){
		//				render::drawString(x + positions[i * 2] * posFactor, y + positions[i * 2 + 1] * posFactor, 0.06, options[i], colorText);
		//			}
		//			else{
		//				render::drawString(x + positions[i * 2] * posFactor, y + positions[i * 2 + 1] * posFactor, 0.05, options[i], colorText);
		//			}
		//		}
	}
}

void WidgetRadialMenu::handleMouseButtonEvent(int i, float, float, bool pressed ){
	if(callbackFunction != NULL && i == 1 && !pressed){
		callbackFunction(selected->name, callbackFunctionData);
		//		callbackFunction(options[selected], callbackFunctionData);
	}
}

void WidgetRadialMenu::handleMouseMotionEvent(float x, float y){
	selectionDepth = hypot(x - getX(), y - getY()) > radius ? 1 : 0;

	RadialMenuTree *t;

	float angle = atan2(x - getX(), y - getY());
	if(angle < 0)
		angle += (2 * 3.1415);
	int i = (static_cast<int>(angle / options.sliceSize));
	if(i >= 0 && i < numOptions){
		if(selectionDepth == 0){
			if(selected != &options)
				selected->close();
			selected = &options.children[i];
		}
		else{
			t = &options.children[i];
			if(t != selected->parent){
				if(selected->parent != &options)
					selected->parent->close();
			}
			t->open();
			float angle2 = atan2(x - (t->x + getX()), y - (t->y + getY()));
			angle2 -= t->angle;
			while(angle2 < -3.1415)
				angle2 += 2 * 3.1415;
			while(angle2 > 3.1415)
				angle2 -= 2 * 3.1415;

			angle2 += subAngle / 2;

			int h = (static_cast<int>(angle2 / t->sliceSize));
			if(h >= 0 && h < t->getShallowSize()){
				selected = &t->children[h];
			}
		}
	}

	//	selected = (static_cast<int>(angle / sliceSize));
}

void WidgetRadialMenu::handleKeyEvent(int, int, bool){

}

void WidgetRadialMenu::setCallbackFunction(void(cf)(std::string, void*), void *data){
	callbackFunction = cf;
	callbackFunctionData = data;
}


void WidgetRadialMenu::setOptions(RadialMenuTree tree){

	options = tree;
	options.rebuildParents();

	radius = 0.2 * (options.getShallowSize() / 6 + 1);

	numOptions = options.getShallowSize();
	options.sliceSize = 2 * 3.1415 / options.getShallowSize();
	float pos = options.sliceSize / 2;
	float subPos = 0;

	for(unsigned int i = 0; i < options.children.size(); i++){
		RadialMenuTree *t = &options.children[i];
		t->angle = pos;
		t->x = sin(pos) * radius;
		t->y = cos(pos) * radius;

		if(t->children.size() > 0){
			t->sliceSize =  subAngle / t->children.size();
			subPos =  -subAngle / 2 + t->sliceSize / 2;
			for(unsigned int h = 0; h < t->children.size(); h++){
				RadialMenuTree *c = &t->children[h];
				c->x = sin(pos + subPos) * radius;//subPos
				c->y = cos(pos + subPos) * radius; //pos - subAngle / 2 +
				c->angle = subPos;
				subPos += t->sliceSize;
			}
		}

		pos += options.sliceSize;
	}

	//	for(RadialMenuTree t : options.children){
	//		t.x = sin(pos) * radius;
	//		t.y = cos(pos) * radius;
	//		pos += sliceSize;
	//	}


	selected = &options.children[0];
	//	options = new std::string[o.size()];
	//	for(unsigned int i = 0; i < o.size(); i++){
	//		options[i] = o[i];
	//	}
	//	numOptions = o.size();
	//	sliceSize = 2 * 3.1415 / numOptions;
	//	float pos = sliceSize / 2;
	//	positions = new float[o.size() * 2];
	//	for(int i = 0; i < numOptions; i++){
	//		positions[i * 2] = sin(pos) * radius;
	//		positions[i * 2 + 1] = cos(pos) * radius;
	//		pos += sliceSize;
	//	}
	//	selected = 0;
}

std::string WidgetRadialMenu::getSelectedOption(){
	if(numOptions > 0){
		if(selected->parent == &options)
			return selected->name;
		else{
			return selected->parent->name + "-" + selected->name;
		}

	}
	else {
		return "";
	}
}

} /* namespace gui */
