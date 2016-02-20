/*
 * WidgetTextureSelect.cpp
 *
 *  Created on: Feb 21, 2015
 *      Author: dwarf
 */

#include <ScreenTextureSelect.h>
#include "Renderer.h"
#include "BlockTextureMap.h"
#include <string>
#include "Warp.h"

#include "Widget.h"

namespace gui {

render::Color colorBackgroundTexture(0.7, 0.7, 0.7, 0.7);
render::Color colorHighlightTexture;

float eventX = 0;
float eventY = 0;
int eventI = 0;

WidgetTextureSelect::WidgetTextureSelect(ToolTexture *t) : Widget() {
	setFocusable(false);
	textureTool = t;
}

WidgetTextureSelect::~WidgetTextureSelect() {

}

void WidgetTextureSelect::render(float updateFactor){
	if(getEditMode()){

		//TODO update rendering method
		renderer->drawRect(0, 0, getWidth(), getHeight(), 0, colorBackgroundTexture);
		int i = 0;
		int maxWidth = (getWidth() - 0.12) / 0.11;
		for(auto a : blockTextureMap){ //find a faster way to iterate throug all loaded blocks
			if(render::getTexture(std::string("block") + std::to_string(a.second)) != NULL){ // TODO think about possibly faster way to check if textuer exists
				if(a.second == textureTool->getCurrentTexture()){
					render::drawRect(getLeft() + 0.06 + (i % maxWidth) * 0.11, getTop() - 0.06 - (i / maxWidth) * 0.11, 0.11, 0.11, 0, colorHighlightTexture);
				}
				render::drawSprite(getLeft() + 0.06 + (i % maxWidth) * 0.11, getTop() - 0.06 - (i / maxWidth) * 0.11, 0.1, 0.1, 0, std::string("block") + std::to_string(a.second));
				i++;
			}
		}

//		render::drawString(getLeft(), getBottom(), 0.05, std::to_string(eventX)); TODO properly remove debugging and event variables when they are no longer needed
//		render::drawString(getLeft(), getBottom() - 0.06, 0.05, std::to_string(eventY));
//		render::drawString(getLeft(), getBottom() - 0.12, 0.05, std::to_string(eventI));
	}
}

void WidgetTextureSelect::handleMouseButtonEvent(int button, float x, float y, bool pressed){



	int maxWidth = (getWidth() - 0.12) / 0.11;
	x = x - getLeft();
	y = getHeight() - (y - getBottom());
	if(x > 0.01 && x < getWidth() - 0.01 && y > 0.01 && y < getHeight() - 0.01){
		x -= 0.01;
		y -= 0.01;
		eventX = x;
		eventY = y;
		int i = static_cast<int>(x / 0.11) + static_cast<int>(y / 0.11) * maxWidth;
		eventI = i;
		for(auto a : blockTextureMap){
			if(render::getTexture(std::string("block") + std::to_string(a.second)) == NULL){
				if(i <= a.second){
					i++;
				}
				else{
					break;
				}
			}
		}
		i = (i + 1) % blockTextureMap.size();
		textureTool->setCurrentTexture(i);
	}
}

} /* namespace gui */
