/*
 * Renderable.h
 *
 *  Created on: Nov 21, 2014
 *      Author: dwarf
 */

#ifndef RENDERABLE_H_
#define RENDERABLE_H_

#include "Renderer.h"

namespace render {


class Renderable {
public:
	Renderable();
	Renderable(float x, float y);
	Renderable(float x, float y, render::Color color);
	virtual ~Renderable();
	virtual void render();
	float x, y;
	Color color;
	void setSustain(bool s);
	bool getSustain();
private:
	bool sustain;
};

}

#endif /* RENDERABLE_H_ */
