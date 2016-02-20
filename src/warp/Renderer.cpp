/*
 * Renderer.cpp
 *
 *  Created on: Dec 2, 2014
 *      Author: dwarf
 */

#include "Warp.h"
#include "Renderable.h"
#include "RenderableSprite.h"
#include "Renderer.h"
#include "RenderableLine.h"
#include "RenderableRect.h"
#include "RenderableString.h"
#include "RenderableCurve.h"
#include "RenderableScissor.h"
#include "FBOHandler.h"
#include <iostream>
#include <cmath>

namespace render{

Mutex renderablesMutex;
RenderTarget target = WORLD;

void drawRenderable(Renderable *r){
	addRenderable(r, target);
}

void drawSprite(float x, float y, float width, float height, float rotation, std::string texture){
	addRenderable(new RenderableSprite(x, y, width, height, rotation, texture), target);
}

void drawLine(float x, float y, float x2, float y2, float thickness, Color c){
	addRenderable(new RenderableLine(x, y, x2, y2, thickness, c), target);
}

void drawRect(float x, float y, float width, float height, float rot, Color c){
	addRenderable(new RenderableRect(x, y, width, height, rot, c), target);
}

void drawCurve(float x1, float y1, float x2, float y2, float dx1, float dy1, float dx2, float dy2, float thickness, Color c){
	addRenderable(new RenderableCurve(x1, x2, y1, y2, dx1, dx2, dy1, dy2, thickness, c), target);
}

void drawRectOutline(float x, float y, float width, float height, float rot, float thickness, Color c){
	float halfwidth = width / 2;
	float halfheight = height / 2;
	float halfthickness = thickness / 2;
	drawRect(x - halfwidth + halfthickness, y, thickness, height, 0, c);
	drawRect(x, y + halfheight - halfthickness, width - 2 * thickness, thickness, 0, c);
	drawRect(x + halfwidth - halfthickness, y, thickness, height, 0, c);
	drawRect(x, y - halfheight + halfthickness, width - 2 * thickness, thickness, 0, c);
}

void drawCircle(float x, float y, float radius, Color c){
	float angle = 0;
	float angleStep = 3.1415 * 2 / 16;
	for(int i = 0; i < 16; i++){
		drawLine(x + sin(angle) * radius, y + cos(angle) * radius, x + sin(angle + angleStep) * radius, y + cos(angle + angleStep) * radius, 0.1, c);
		angle += angleStep;
	}
}

void drawString(float x, float y, float lineheight, std::string string, Color c){
	addRenderable(new RenderableString(x, y, lineheight, string, c), target);
}

void setRenderTarget(RenderTarget t){
	target = t;
}

void setScissorArea(float x, float y, float width, float height){
	addRenderable(new RenderableScissor(x, y, width, height), target);
}

RenderableSprite *createRenderableSprite(float x, float y, float width, float height, float rotation, std::string texture){
	return new RenderableSprite(x, y, width, height, rotation, texture);
}

RenderableSprite *createRenderableSprite(float x, float y, float width, float height, float rotation, render::Texture *texture){
	return new RenderableSprite(x, y, width, height, rotation, texture);
}

}


