/*
 * WidgetToggle.h
 *
 *  Created on: Aug 26, 2015
 *      Author: dwarf
 */

#ifndef WIDGETTOGGLE_H_
#define WIDGETTOGGLE_H_

#include <Widget.h>

namespace warp{
namespace gui {


class ToggleListener{
public:
	ToggleListener(){}
	virtual ~ToggleListener(){}
	virtual void onToggled(std::string name, bool state){}
};

class WidgetToggle: public Widget {
public:
	WidgetToggle(std::string name, bool initState = false);
	virtual ~WidgetToggle();
	void handleMouseButtonEvent(int button, float x, float y, bool pressed);
	void render(float updateFactor);
	void setListener(ToggleListener *l);
	void setState(bool s);
private:
	bool state;
	std::string name;
	ToggleListener *listener;
};

} /* namespace warp{
namespace gui */

}
#endif /* WIDGETTOGGLE_H_ */
