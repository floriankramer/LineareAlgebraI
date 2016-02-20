/*
 * Widget.h
 *
 *  Created on: Dec 22, 2014
 *      Author: dwarf
 */

#ifndef GUI_WIDGET_H_
#define GUI_WIDGET_H_

#include <vector>
#include "Logger.h"
#include "Vec2f.h"
#include "Renderer.h"

namespace gui {

class Widget;

class WidgetRenderer{

public:
	WidgetRenderer(Widget *owner);
	virtual ~WidgetRenderer();

	void drawSprite(float x, float y, float width, float height, float rotation, std::string texture);
	void drawSpriteCentered(float x, float y, float width, float height, float rotation, std::string texture);
	void drawLine(float x, float y, float x2, float y2, float thickness, render::Color c = render::white);
	void drawRect(float x, float y, float width, float height, float rot, render::Color c = render::white);
	void drawCurve(float x1, float y1, float x2, float y2, float dx1, float dy1, float dx2, float dy2, float thickness, render::Color c = render::white);
	void drawRectOutline(float x, float y, float width, float height, float rot, float thickness, render::Color c = render::white);
	void drawString(float x, float y, float lineheight, std::string string, render::Color c = render::white);
private:
	Widget *owner;

};


class Widget {
public:
	Widget();
	virtual ~Widget();
	void setListenToMouse(bool);
	bool isListeningToMouse() ;
	void setListenToKeys(bool);
	bool isListeningToKeys();
	void setHeight(float);
	void setPaddingY(float) ;
	float getPaddingY();
	void setPaddingX(float);
	float getPaddingX();
	void setWidth(float);
	void setLocation(float, float);
	float getHeight();
	float getWidth();
	void getWidgetsAt(float, float, std::vector<Widget*> *);


	virtual void handleKeyEvent(int, int, bool);
	virtual void handleMouseButtonEvent(int button, float x, float y, bool pressed);
	virtual void handleMouseMotionEvent(float x, float y);
	virtual void handleAxisEvent(int axis, float value);
	virtual void handleButtonEvent(int button, bool pressed);
	virtual void handleMouseWheelEvent(float value);
	virtual void handleTextEvent(std::string s);

	virtual void render(float updateFactor);
	void renderConsole(float updateFactor);
	void handleInput();
	void addWidget(Widget*);
	void removeWidget(Widget*);
	bool isFocused();
	void setHasFocus(bool);
	bool hasChild(Widget *w);
	Widget* getParent();
	void setParent(Widget *w);
	std::vector<Widget*>* getChildren();
	bool isFocusable();

	float getRight();
	float getBottom();
	float getCenterX();
	float getCenterY();
	float getLeft();
	float getTop();

	float getAbsoluteLeft();
	float getAbsoluteTop();

	void handleEditModeChange();

	virtual void onFocusChanged();
	virtual void onEditModeChanged(bool b);

	virtual void onAdd(Widget *w);
	/**
	 * if the event is used, the widget has to be removed after the event has been handled
	 */
	virtual void onRemove(Widget *w);
	void doRemove(Widget *w);
	void setConsoleActive(bool a);

protected:
	void setFocusable(bool f);

	WidgetRenderer *renderer;

private:




	float x, y, width, height, paddingX, paddingY;
	std::vector<Widget*> children;
	Widget* parent;
	bool listenToKeys, listenToMouse, hasFocus, focusable;
};

extern Logger guiLogger;
void setEditMode(bool);
bool getEditMode();
void setFocus(Widget *);
Widget *getFocused();
bool getConsoleActive();



} /* namespace game */

#endif /* GUI_WIDGET_H_ */
