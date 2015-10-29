/*
 * ToolLight.h
 *
 *  Created on: Feb 6, 2015
 *      Author: dwarf
 */

#ifndef GUI_TOOLLIGHT_H_
#define GUI_TOOLLIGHT_H_

#include "Tool.h"
#include "LightHandler.h"

namespace gui {

class ToolLight : public Tool{
public:
	ToolLight();
	virtual ~ToolLight();
	void begin(float, float, int);
	void step(float, float, int);
	void stop(float, float, int);
	std::string getName();
private:
	render::light::Light *current;
	int currentIndex;
};

} /* namespace gui */

#endif /* GUI_TOOLLIGHT_H_ */
