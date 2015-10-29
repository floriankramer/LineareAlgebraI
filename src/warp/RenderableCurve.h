/*
 * RenderableCurve.h
 *
 *  Created on: Mar 6, 2015
 *      Author: dwarf
 */

#ifndef RENDERABLECURVE_H_
#define RENDERABLECURVE_H_

#include <Renderable.h>

namespace render {

class RenderableCurve: public Renderable {
public:
	RenderableCurve(float x1, float x2, float y1, float y2, float dx1, float dx2, float dy1, float dy2, float thickness, Color c);
	virtual ~RenderableCurve();
	void render();
private:
	float x2, y2, thickness, dx1, dx2, dy1, dy2;
};

} /* namespace render */

#endif /* RENDERABLECURVE_H_ */
