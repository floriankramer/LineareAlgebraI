/*
 * RenderableScissor.h
 *
 *  Created on: Oct 30, 2015
 *      Author: dwarf
 */

#ifndef RENDERABLESCISSOR_H_
#define RENDERABLESCISSOR_H_

#include "Renderable.h"

namespace render {

class RenderableScissor: public Renderable {
public:
	RenderableScissor(float x, float y, float width, float height);
	virtual ~RenderableScissor();
	void render();
private:
	float width, height;

};

} /* namespace gui */

#endif /* RENDERABLESCISSOR_H_ */
