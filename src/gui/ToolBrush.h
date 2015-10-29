/*
 * ToolBrush.h
 *
 *  Created on: Feb 6, 2015
 *      Author: dwarf
 */

#ifndef GUI_TOOLBRUSH_H_
#define GUI_TOOLBRUSH_H_

#include <Tool.h>

namespace gui {

class ToolBrush: public Tool {
public:
	ToolBrush();
	virtual ~ToolBrush();
	void begin(float, float, int);
	void step(float, float, int);
	void stop(float, float, int);
	void render(float updateFactor);
	std::string getName();

};

} /* namespace gui */

#endif /* GUI_TOOLBRUSH_H_ */
