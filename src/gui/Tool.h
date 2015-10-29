/*
 * Tool.h
 *
 *  Created on: Feb 6, 2015
 *      Author: dwarf
 */

#ifndef GUI_TOOL_H_
#define GUI_TOOL_H_

#include <string>
#include "Widget.h"
#include "Renderer.h"

namespace gui {

extern render::Color toolOverlayColor;

class Tool {
public:
	Tool();
	virtual ~Tool();
	virtual void begin(float, float, int);
	virtual void step(float, float, int);
	virtual void stop(float, float, int);
	virtual std::string getName();
	virtual void onKeyPress(int button, int modifier, bool pressed, Widget *parent);
	virtual std::string setParameter(std::string name, std::string value);
	virtual void render(float updateFactor);

};

} /* namespace gui */

#endif /* GUI_TOOL_H_ */
