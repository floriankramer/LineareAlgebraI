/*
 * WidgetRadialMenu.h
 *
 *  Created on: Feb 15, 2015
 *      Author: dwarf
 */

#ifndef GUI_WIDGETRADIALMENU_H_
#define GUI_WIDGETRADIALMENU_H_

#include <Widget.h>
#include <string>
#include <vector>
#include <cmath>
#include <Renderable.h>

namespace gui {

struct RadialMenuTree{

	std::vector<RadialMenuTree> children;
	std::string name;
	RadialMenuTree *parent;
	float sliceSize;

	std::string textureName;
	float textureWidth, textureHeight;

	float x, y;
	float animValue;
	float posFactor;
	int animState;
	float angle;

	RadialMenuTree(std::string name){
		this->name = name;
		x = y = 0;
		parent = 0;
		sliceSize = 1;
		animValue = 0;
		posFactor = 0;
		animState = 0;
		angle = 0;
		textureWidth = 1;
		textureHeight = 1;
	}

	RadialMenuTree(){
		name = std::string("empty");
		x = y = 0;
		parent = 0;
		sliceSize = 1;
		animValue = 0;
		posFactor = 0;
		animState = 0;
		angle = 0;
		textureWidth = 1;
		textureHeight = 1;
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
//		animValue = 0;
	}

	void close(){
		animState = 2;
//		animValue = 1;
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
