/*
 * Vec2f.cpp
 *
 *  Created on: Nov 20, 2014
 *      Author: dwarf
 */

#include "Vec2f.h"

namespace warp {



Vec2f::Vec2f() {
	y = 0;
	x = 0;

}

Vec2f::Vec2f(float x, float y){
	this->x = x;
	this->y = y;
}

Vec2f::~Vec2f() {

}

} /* namespace warp */
