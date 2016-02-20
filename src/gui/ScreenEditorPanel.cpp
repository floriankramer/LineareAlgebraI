/*
 * WidgetEditorPanel.cpp
 *
 *  Created on: Apr 27, 2015
 *      Author: dwarf
 */

#include <ScreenEditorPanel.h>

namespace gui {

WidgetEditorPanel::WidgetEditorPanel() : Widget() {

}

WidgetEditorPanel::~WidgetEditorPanel() {

}

void WidgetEditorPanel::render(float updateFactor){
	renderer->drawRect(0, 0, getWidth(), getHeight(), 0);
}

} /* namespace gui */
