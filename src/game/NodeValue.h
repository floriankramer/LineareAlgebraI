/*
 * NodeValue.h
 *
 *  Created on: Mar 11, 2015
 *      Author: dwarf
 */

#ifndef GAME_NODEVALUE_H_
#define GAME_NODEVALUE_H_

#include <Node.h>
#include "WidgetTextField.h"

namespace game {

class NodeValue: public Node, public gui::TextFieldListener{
public:
	NodeValue();
	virtual ~NodeValue();

	void onTextChanged(std::string text);
	void render(bool editMode, float updateFactor);

private:
	gui::WidgetTextField textField;
	bool addedTextField;

};

} /* namespace game */

#endif /* GAME_NODEVALUE_H_ */
