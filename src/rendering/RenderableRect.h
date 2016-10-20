/*
 * RenderableRect.h
 *
 *  Created on: Dec 19, 2014
 *      Author: dwarf
 */

#ifndef RENDERABLERECT_H_
#define RENDERABLERECT_H_

#include "Renderable.h"

namespace warp {

class RenderableRect: public Renderable {
public:
	RenderableRect(float, float, float, float, float, Color c);
	virtual ~RenderableRect();
	void render();
private:
	float width, height, rot;
};

} /* namespace warp */

#endif /* RENDERABLERECT_H_ */
