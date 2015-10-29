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

struct WidgetRenderer{

	void drawSprite(float x, float y, float width, float height, float rotation, std::string texture){
		render::drawSprite(x + left + width / 2, y + bottom + height / 2, width, height, rotation, texture);
	}

	void drawSpriteCentered(float x, float y, float width, float height, float rotation, std::string texture){
		render::drawSprite(x + left, y + bottom, width, height, rotation, texture);
	}

	void drawLine(float x, float y, float x2, float y2, float thickness, render::Color c = render::white){
		render::drawLine(left + x, bottom + y, left + x2,  bottom + y2, thickness, c);
	}

	void drawRect(float x, float y, float width, float height, float rot, render::Color c = render::white){
		render::drawRect(x + left + width / 2, y + bottom + height / 2, width, height, rot, c);
	}

	void drawCurve(float x1, float y1, float x2, float y2, float dx1, float dy1, float dx2, float dy2, float thickness, render::Color c = render::white){
		render::drawCurve(x1, y1, x2, y2, dx1, dy1, dx2, dy2, thickness, c);
	}

	void drawRectOutline(float x, float y, float width, float height, float rot, float thickness, render::Color c = render::white){
		render::drawRectOutline(x + left + width / 2, y + bottom + height / 2, width, height, rot, thickness, c);
	}

	void drawString(float x, float y, float lineheight, std::string string, render::Color c = render::white){
		render::drawString(x + left, y + bottom, lineheight, string, c);
	}

	float getWidth(){
		return right - left;
	}

	float getHeight(){
		return top - bottom;
	}


	float left, bottom, right, top;
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
	float getY();
	float getX();
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
	void handleInput();
	void addWidget(Widget*);
	void removeWidget(Widget*);
	bool isFocused();
	void setHasFocus(bool);
	float getLeft();
	float getRight();
	float getTop();
	float getBottom();
	bool hasChild(Widget *w);
	std::vector<Widget*>* getParents();
	std::vector<Widget*>* getChildren();
	bool isFocusable();

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

	WidgetRenderer renderer;

private:




	float x, y, width, height, paddingX, paddingY, left, right, top, bottom;
	std::vector<Widget*> children;
	std::vector<Widget*> parents;
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
