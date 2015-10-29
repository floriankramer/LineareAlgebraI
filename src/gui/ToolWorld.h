/*
 * ToolWorld.h
 *
 *  Created on: Jul 4, 2015
 *      Author: dwarf
 */

#ifndef GUI_TOOLWORLD_H_
#define GUI_TOOLWORLD_H_

#include <Tool.h>

namespace gui {

class ToolWorld: public Tool {
public:
	ToolWorld();
	virtual ~ToolWorld();
	void begin(float, float, int);
	void step(float, float, int);
	void stop(float, float, int);
	std::string setParameter(std::string name, std::string value);
	std::string getName();
	void render(float upateFactor);
private:
	std::string path;
	int width, height;
};

} /* namespace gui */

#endif /* GUI_TOOLWORLD_H_ */
