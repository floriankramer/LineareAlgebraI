/*
 * Renderable.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: dwarf
 */

#include <SDL2/SDL.h>
//#include <SDL2/SDL_opengl.h>
#include "glewWrapper.h"
#include "Renderable.h"
#include "Warp.h"

namespace render {

Renderable::Renderable() : x(0), y(0), sustain(false){

}

Renderable::Renderable(float x, float y): x(x), y(y), sustain(false){

}

Renderable::Renderable(float x, float y, render::Color color): x(x), y(y), color(color), sustain(false){

}

Renderable::~Renderable() {

}

void Renderable::setSustain(bool s){
	sustain = s;
}

bool Renderable::getSustain(){
	return sustain;
}

void Renderable::render(){


	glBindTexture(GL_TEXTURE_2D, getTexture("test")->id);

	glLoadIdentity();


	glBegin(GL_QUADS);
	glTexCoord2i(0, 0);
	glVertex2f(-1, -1);
	glTexCoord2i(1, 0);
	glVertex2f(1, -1);
	glTexCoord2i(1, 1);
	glVertex2f(1, 1);
	glTexCoord2i(0, 1);
	glVertex2f(-1, 1);
	glEnd();
}

}
