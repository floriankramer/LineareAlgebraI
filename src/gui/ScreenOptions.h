/*
 * WidgetOptions.h
 *
 *  Created on: Aug 26, 2015
 *      Author: dwarf
 */

#ifndef SCREENOPTIONS_H_
#define SCREENOPTIONS_H_

#include <Widget.h>
#include <WidgetButton.h>
#include "WidgetToggle.h"
#include "WidgetTextField.h"
#include "WidgetSlider.h"

namespace gui {

class WidgetOptions: public Widget, public ButtonListener, public ToggleListener {
public:
	WidgetOptions();
	virtual ~WidgetOptions();
	void onButtonPressed(std::string text);
	void onToggled(std::string name, bool state);
	void render(float updateFactor);
	void updateValues();
private:
	void applyChanges();
private:
	WidgetButton buttonBack;
	WidgetButton buttonApply;
	WidgetToggle toggleFullscreen;
	WidgetTextField textResolution;
	WidgetSlider masterVolume;
	WidgetSlider musicVolume;
};

} /* namespace gui */

#endif /* SCREENOPTIONS_H_ */
