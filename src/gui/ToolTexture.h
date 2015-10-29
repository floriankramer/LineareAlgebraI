/*
 * ToolTexture.h
 *
 *  Created on: Feb 21, 2015
 *      Author: dwarf
 */

#ifndef GUI_TOOLTEXTURE_H_
#define GUI_TOOLTEXTURE_H_

#include <Tool.h>

namespace gui {

class ToolTexture: public Tool {
public:
	ToolTexture();
	virtual ~ToolTexture();
	void begin(float, float, int);
	void step(float, float, int);
	void stop(float, float, int);
	int getCurrentTexture();
	std::string getName();
	void setCurrentTexture(int i);
private:
	int currentTexture;
};

} /* namespace gui */

#endif /* GUI_TOOLTEXTURE_H_ */
