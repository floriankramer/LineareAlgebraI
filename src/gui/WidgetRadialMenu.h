/*
 * WidgetRadialMenu.h
 *
 *  Created on: Feb 15, 2015
 *      Author: dwarf
 */

#ifndef GUI_WIDGETRADIALMENU_H_
#define GUI_WIDGETRADIALMENU_H_

#include "Widget.h"
#include <string>
#include <vector>
#include <cmath>
#include "Renderable.h"

namespace gui {

/* RadialmenuTree stores data describing the items in the RadialMenu. 
 * Furthermore it provides simple Functions to modify the tree and
 * query its properties. The animation data is stored in here as well.
 */
struct RadialMenuTree{

	std::vector<RadialMenuTree> children;
	std::string name;
	RadialMenuTree *parent;
	// stores the angle in radian that a single, toplevel fork of the tree uses
	float sliceSize;

	// ???
	std::string textureName;
	float textureWidth, textureHeight;

	float x, y;

	// value between 0 and 1 determining the progress of the animation
	float animValue;

	// Determines how far out the leaves should be in relation to their maximum distance to the root
	// lies between 0 and 1
	float posFactor;

	// Determins opening or closing animation. Is 1 for opening, 2 for closing
	int animState;

	// ???
	float angle;

	RadialMenuTree(std::string name){
		this->name = name;
		x = y = 0;
		parent = 0;
		sliceSize = 1;
		animValue = 0;
		posFactor = 0;
		animState = 0;	
		textureWidth = 1;
		textureHeight = 1;
		angle = 0;
	}

	RadialMenuTree(){
		name = std::string("empty");
		x = y = 0;
		parent = 0;
		sliceSize = 1;
		animValue = 0;
		posFactor = 0;
		animState = 0;
		textureWidth = 1;
		textureHeight = 1;
		angle = 0;
	}

	int getShallowSize(){
		return children.size();
	}

	void add(RadialMenuTree t){
		children.push_back(t);
		t.parent = this;
	}

	void rebuildParents(){
		for(unsigned int i = 0; i < children.size(); i++){
			children[i].parent = this;
			children[i].rebuildParents();
		}
	}

	void open(){
		animState = 1;
	}

	void close(){
		animState = 2;
	}

	void updateAnim(float updateFactor){
		if(animState == 1){
			animValue += updateFactor * 4;//(updateFactor / 0.05) * animValue * (1.05 - animValue);
			if(animValue >= 1){
				animValue = 1;
				animState = 0;
			}
			posFactor = sin(animValue * 3.1415 / 2);
		}
		else if(animState == 2){
			animValue -= updateFactor * 5;
			if(animValue <= 0){
				animValue = 0;
				animState = 0;
			}
			posFactor = sin(animValue * 3.1415 / 2);
		}
		for(unsigned int i = 0; i < children.size(); i++){
			children[i].updateAnim(updateFactor);
		}
	}

	void setTexture(std::string t, float width, float height){
		textureName = t;
		textureWidth = width;
		textureHeight = height;
	}

};

class WidgetRadialMenu: public Widget {
public:
	WidgetRadialMenu();
	virtual ~WidgetRadialMenu();
	void render(float updateFactor);
	void handleMouseButtonEvent(int, float, float, bool);
	void handleKeyEvent(int, int, bool);
	void setCallbackFunction(void(cf)(std::string, void*), void*);
	void setOptions(RadialMenuTree t);
	void handleMouseMotionEvent(float, float);
	void onAdd(Widget *w);
	void onRemove(Widget *w);
	std::string getSelectedOption();
private:
	RadialMenuTree options;

	std::vector<RadialMenuTree *> renderTree;

	void (*callbackFunction)(std::string, void*);
	float radius;
	int numOptions;
	RadialMenuTree *selected;
	void *callbackFunctionData;
	int selectionDepth = 0;
};

} /* namespace gui */

#endif /* GUI_WIDGETRADIALMENU_H_ */
