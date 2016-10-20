/*
 * Keyboard.h
 *
 *  Created on: Dec 23, 2014
 *      Author: dwarf
 */

#ifndef GUI_KEYBOARD_H_
#define GUI_KEYBOARD_H_

namespace warp{
namespace gui{

void initKeyboard();
void destroyKeyboard();
void updateKey(int key, bool state);
bool isKeyDown(int key);
}

}
#endif /* GUI_KEYBOARD_H_ */
