/*
 * Keyboard.cpp
 *
 *  Created on: Dec 23, 2014
 *      Author: dwarf
 */

#include "Keyboard.h"
#include "Widget.h"
#include <iostream>

#define MAX_KEY_ID 256

namespace gui{
bool* keys;

void initKeyboard(){
	keys = new bool[MAX_KEY_ID];//(char*) malloc(256 * sizeof(keys));
	for(int i = 0; i < MAX_KEY_ID; i++){
		keys[i] = false;
	}
}

void updateKey(int key, bool state){
	if(key > 0 && key < MAX_KEY_ID)
		keys[key] = state;
}

void destroyKeyboard(){
	delete[] keys;
}

bool isKeyDown(int key){
	if(key > 0 && key < MAX_KEY_ID){
		return keys[key];
	}
	else{
		guiLogger.log("key not tracked");
		return false;
	}
}

}
