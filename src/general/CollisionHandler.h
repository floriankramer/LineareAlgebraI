/*
 * CollisionHandler.h
 *
 *  Created on: Dec 20, 2014
 *      Author: dwarf
 */

#ifndef COLLISIONHANDLER_H_
#define COLLISIONHANDLER_H_

namespace warp{


struct Rect{
	float x,
	y,
	width,
	height;
};

bool rectsOverlap(Rect r1, Rect r2);

bool rectsOverlap(float, float, float, float, float, float, float, float);
bool pointInRect(float, float, float, float, float, float);
bool atop(Rect object, Rect base, float margin);
bool atop(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2, float margin);
bool linesIntersect(float x1, float x2, float y1, float y2, float x3, float x4, float y3, float y4, float margin=0);
bool lineIntersectsRect(float q1x, float x2, float q1y, float y2, float llx, float lly, float urx, float ury);
float *getLineRectIntersection(float q1x, float x2, float q1y, float y2, float llx, float lly, float urx, float ury);
float* getLineIntersection(float q1x, float x2, float q1y, float y2, float q2x, float x4, float q2y, float y4, float margin=0);

}



#endif /* COLLISIONHANDLER_H_ */
