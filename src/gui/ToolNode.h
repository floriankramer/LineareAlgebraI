/*
 * ToolNode.h
 *
 *  Created on: Feb 9, 2015
 *      Author: dwarf
 */

#ifndef GUI_TOOLNODE_H_
#define GUI_TOOLNODE_H_

#include <Tool.h>
#include "WidgetRadialMenu.h"

namespace gui {

class ToolNode: public Tool {
public:
	ToolNode();
	virtual ~ToolNode();
	void begin(float, float, int);
	void step(float, float, int);
	void stop(float, float, int);
	std::string getName();
	void onKeyPress(int button, int modifier, bool pressed, Widget *parent);
	void setNodeType(int i);
	void render(float upateFactor);
private:
	int lastNode;
	float beginX, beginY;
	int nodeType;
	int grabbedNodeGoal;
	int grabbedNode;
	WidgetRadialMenu typeSelectMenu;
	bool snapToX, snapToY;
	int lastButton;
	void setupRadialMenu();

	int handle, handleDirection;
};

} /* namespace gui */

#endif /* GUI_TOOLNODE_H_ */
