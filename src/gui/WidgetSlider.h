/*
 * WidgetSlider.h
 *
 *  Created on: Oct 27, 2015
 *      Author: dwarf
 */

#ifndef WIDGETSLIDER_H_
#define WIDGETSLIDER_H_

#include <Widget.h>

namespace warp{
namespace gui {

class WidgetSlider: public Widget {
public:
	WidgetSlider();
	virtual ~WidgetSlider();
	virtual void render(float updateFactor);
	virtual void handleMouseButtonEvent(int button, float x, float y, bool pressed);
	virtual void handleMouseMotionEvent(float x, float y);
	virtual void onFocusChanged();
	void setValue(float v);
	float getValue();
private:
	float value;
};

} /* namespace warp{
namespace gui */

}
#endif /* WIDGETSLIDER_H_ */
