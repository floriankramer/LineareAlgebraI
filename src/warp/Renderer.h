/*
 * Renderer.h
 *
 *  Created on: Dec 2, 2014
 *      Author: dwarf
 */

#ifndef RENDERER_H_
#define RENDERER_H_

#include <string>
#include "../Mutex.h"




namespace render{

class Renderable;
class RenderableSprite;
struct Texture;

struct Color{
	float r = 1, g = 1, b = 1, a = 1;
	Color(){}
	Color(float r, float g, float b){
		this->r = r;
		this->g = g;
		this->b = b;
	}
	Color(float r, float g, float b, float a){
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}
};

const static Color white;

enum RenderTarget{WORLD, HUD};

void drawRenderable(Renderable *r);
void drawSprite(float x, float y, float width, float height, float rotation, std::string texture);
void drawLine(float x, float y, float x2, float y2, float thickness, Color c = white);
void drawRect(float x, float y, float width, float height, float rot, Color c = white);
void drawRectOutline(float x, float y, float width, float height, float rot, float thickness, Color c = white);
void drawString(float x, float y, float lineheight, std::string string, Color c = white);
void drawCurve(float x1, float y1, float x2, float y2, float dx1, float dy1, float dx2, float dy2, float thickness, Color c = white);
void drawCircle(float x1, float x2, float radius, Color c = white);
void setScissorArea(float x, float y, float width, float height);
void setRenderTarget(RenderTarget t);

RenderableSprite *createRenderableSprite(float x, float y, float width, float height, float rotation, std::string texture);
RenderableSprite *createRenderableSprite(float x, float y, float width, float height, float rotation, Texture *texture);


extern Mutex renderablesMutex;


}

#endif


