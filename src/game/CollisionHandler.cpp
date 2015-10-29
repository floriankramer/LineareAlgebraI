/*
 * CollisionHandler.cpp
 *
 *  Created on: Dec 20, 2014
 *      Author: dwarf
 */

#include "CollisionHandler.h"

namespace game{
namespace physics{

bool rectsOverlap(Rect r1, Rect r2){
	return rectsOverlap(r1.x, r1.y, r1.width, r1.height, r2.x, r2.y, r2.width, r2.height);
}

bool atop(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2, float margin){
	return y1 - (y2 + h2) < margin && y1 - (y2 + h2) > -margin && x1 < (x2 + w2) && x1 + w1 > x2;
}

//bool atop(Rect object, Rect base, float margin){ //&& object.y - (base.y + base.height) > 0
//	return object.y - (base.y + base.height) < margin;//  && object.x < base.x + base.width && object.x + object.width > base.x;
//}

bool rectsOverlap(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
	if (x1 > (x2 + w2) || (x1 + w1) < x2) {
		return false;
	}
	if (y1 > (y2 + h2) || (y1 + h1) < y2) {
		return false;
	}
	return true;
}

bool pointInRect(float x1, float y1, float x2, float y2, float w2, float h2){
	if(w2 > 0 && h2 > 0){
		return x1 >= x2 && x1 <= x2 + w2 && y1 >= y2 && y1 <= y2 + h2;
	}
	else{
		if(w2 < 0 && h2 < 0){
			return x1 >= x2 + w2 && x1 <= x2 && y1 >= y2 + h2 && y1 <= y2;
		}
		else if(w2 < 0){
			return x1 >= x2 + w2 && x1 <= x2 && y1 >= y2 && y1 <= y2 + h2;
		}
		else if(h2 < 0){
			return x1 >= x2 && x1 <= x2 + w2 && y1 >= y2 + h2 && y1 <= y2;
		}
		return false;
	}
}

/**
 * @params 	q = base vector of line
 * 			x / y = endpoint of line
 */
bool linesIntersect(float q1x, float x2, float q1y, float y2, float q2x, float x4, float q2y, float y4){
	double v1x = x2 - q1x, v1y = y2 - q1y, v2x = x4 - q2x, v2y = y4 - q2y; //define direction vectos with length of line segment
	double s = (- v1x * (q2y - q1y) + v1y * (q2x - q1x)) / (v1x * v2y - v1y * v2x); //calculate intersection point variable for first line segment
	double t = (- v2x * (q1y - q2y) + v2y * (q1x - q2x)) / (v2x * v1y - v2y * v1x); //calculate intersection point variable for second line segment
	return s >= 0 && s <= 1 && t >= 0 && t <= 1; //s and t are only defined in [0;1], so only if they are in this range there is a solution
}

/**
 * @params 	q = base vector of line
 * 			x / y = endpoint of line
 * 			llx / y coordinates of lower left point
 * 			urx / y coordinates of upper right point
 *
 * checks for line-segment rect intersection, by checking for line intersection between the line and the connections
 * of opposit points of the rect
 */
bool lineIntersectsRect(float q1x, float x2, float q1y, float y2, float llx, float lly, float urx, float ury){
	return linesIntersect(q1x, x2, q1y, y2, llx, urx, lly, ury) || linesIntersect(q1x, x2, q1y, y2, urx, llx, lly, ury);
}

}
}
