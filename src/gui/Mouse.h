/*
 * Mouse.h
 *
 *  Created on: Jan 31, 2015
 *      Author: dwarf
 */

#ifndef GUI_MOUSE_H_
#define GUI_MOUSE_H_

namespace gui{

bool isMouseButtonDown(int);
void updateMouseButton(int, bool);
/**
 * return the current mouse position in screen space
 */
float* getMousePosition();
void updateMousePosition(float x, float y);

}



#endif /* GUI_MOUSE_H_ */
