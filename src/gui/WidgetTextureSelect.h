/*
 * WidgetTextureSelect.h
 *
 *  Created on: Feb 21, 2015
 *      Author: dwarf
 */

#ifndef GUI_WIDGETTEXTURESELECT_H_
#define GUI_WIDGETTEXTURESELECT_H_

#include <Widget.h>
#include <ToolTexture.h>

namespace gui {

class WidgetTextureSelect: public Widget {
public:
	WidgetTextureSelect(ToolTexture *t);
	virtual ~WidgetTextureSelect();
	void render(float updateFactor);
	void handleMouseButtonEvent(int button, float x, float y, bool pressed);
private:
	ToolTexture *textureTool;
};

} /* namespace gui */

#endif /* GUI_WIDGETTEXTURESELECT_H_ */
