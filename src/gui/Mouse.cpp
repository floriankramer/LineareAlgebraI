/*
 * Mouse.cpp
 *
 *  Created on: Jan 31, 2015
 *      Author: dwarf
 */
#include "Mouse.h"
#include <iostream>

namespace warp{
namespace gui{

bool lmb = false, rmb = false, mmb = false;
float mp[2];

float* getMousePosition(){
	return mp;
}

void updateMousePosition(float x, float y){
	mp[0] = x;
	mp[1] = y;
}

bool isMouseButtonDown(int i){
	switch(i){
	case 1:
		return lmb;
		break;
	case 2:
		return rmb;
		break;
	case 3:
		return mmb;
		break;
	default:
		return false;
	}
}
void updateMouseButton(int i, bool b){
	switch(i){
	case 1:
		lmb = b;
		break;
	case 2:
		rmb = b;
		break;
	case 3:
		mmb = b;
		break;
	}
}

}


}
