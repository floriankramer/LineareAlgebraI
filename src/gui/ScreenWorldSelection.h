/*
 * WidgetWorldSelection.h
 *
 *  Created on: Aug 26, 2015
 *      Author: dwarf
 */

#ifndef SCREENWORLDSELECTION_H_
#define SCREENWORLDSELECTION_H_

#include <Widget.h>
#include <WidgetButton.h>
#include <map>
#include <vector>

namespace gui {

class WidgetWorldSelection: public Widget, public ButtonListener {
public:
	WidgetWorldSelection();
	virtual ~WidgetWorldSelection();
	void render(float updateFactor);
	void onButtonPressed(std::string text);
private:
	std::vector<WidgetButton*> buttons;
	std::map<std::string, std::string> nameToPath;

};

} /* namespace gui */

#endif /* SCREENWORLDSELECTION_H_ */
