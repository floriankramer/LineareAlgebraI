/*
 * WidgetTextField.h
 *
 *  Created on: Mar 11, 2015
 *      Author: dwarf
 */

#ifndef GUI_WIDGETTEXTFIELD_H_
#define GUI_WIDGETTEXTFIELD_H_

#include "Widget.h"
#include <string.h>

namespace warp{
namespace gui {

class TextFieldListener{
public:
	virtual ~TextFieldListener(){}
	virtual void onTextChanged(std::string text){}
};

class WidgetTextField : public Widget{
public:
	WidgetTextField();
	WidgetTextField(float width, float height);
	virtual ~WidgetTextField();
	void render(float updateFactor);
	virtual void handleKeyEvent(int, int, bool);
	void setTextFieldListener(TextFieldListener *l);
	void setText(std::string t);
	std::string getText();
private:
	std::string text;
	unsigned int cursorPos;
	int blinkState;
	TextFieldListener *listener;

};

} /* namespace warp{
namespace gui */

}
#endif /* GUI_WIDGETTEXTFIELD_H_ */
