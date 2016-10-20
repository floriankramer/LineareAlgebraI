#ifndef RENDERABLE_CIRCLE_H_
#define RENDERABLE_CIRCLE_H_

#include "Renderable.h"

namespace warp {

class RenderableCircle: public Renderable {
public:
	RenderableCircle(float x, float y, float radius, Color c);
	virtual ~RenderableCircle();
	void render();
private:
	float radius;
};

} /* namespace warp */

#endif
