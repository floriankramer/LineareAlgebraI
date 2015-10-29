/*
 * RenderableLine.h
 *
 *  Created on: Dec 19, 2014
 *      Author: dwarf
 */

#ifndef RENDERABLELINE_H_
#define RENDERABLELINE_H_

#include "Renderable.h"

namespace render {

class RenderableLine: public Renderable {
public:
	RenderableLine(float x1, float x2, float y1, float y2, float thickness, Color c);
	virtual ~RenderableLine();
	void render();
private:
	float x2, y2, thickness;
};

} /* namespace render */

#endif /* RENDERABLELINE_H_ */
