/*
 * RenderableString.h
 *
 *  Created on: Dec 30, 2014
 *      Author: dwarf
 */

#ifndef RENDERABLESTRING_H_
#define RENDERABLESTRING_H_

#include <Renderable.h>
#include <string>

namespace render {

class RenderableString: public render::Renderable {
public:
	RenderableString(float, float, float, std::string, Color c);
	virtual ~RenderableString();
	void render();
private:
	std::string text;
	float x, y;
	float textHeight;
};

} /* namespace game */

#endif /* RENDERABLESTRING_H_ */
