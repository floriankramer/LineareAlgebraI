/*
 * ToolTexture.cpp
 *
 *  Created on: Feb 21, 2015
 *      Author: dwarf
 */

#include <ToolTexture.h>

#include "World.h"

namespace gui {

ToolTexture::ToolTexture() {
	currentTexture = 3;
}

ToolTexture::~ToolTexture() {
}

void ToolTexture::begin(float x, float y, int button){
	if(button == 1 && currentTexture != -1)
		game::world::setBlockTextureAt((int)x, (int)y, currentTexture);
}

void ToolTexture::step(float x, float y, int button){
	if(button == 1 && currentTexture != -1)
		game::world::setBlockTextureAt((int)x, (int)y, currentTexture);
}

void ToolTexture::stop(float x, float y, int button){

}

void ToolTexture::setCurrentTexture(int i){
	currentTexture = i;
}

int ToolTexture::getCurrentTexture(){
	return currentTexture;
}

std::string ToolTexture::getName(){
	return "Texture";
}

} /* namespace gui */
