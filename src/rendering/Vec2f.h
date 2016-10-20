/*
 * Vec2f.h
 *
 *  Created on: Nov 20, 2014
 *      Author: dwarf
 */

#ifndef VEC2F_H_
#define VEC2F_H_

namespace warp {

class Vec2f {
public:

	float x;
	float y;

	Vec2f();
	Vec2f(float, float);
	virtual ~Vec2f();
};

} /* namespace warp */

#endif /* VEC2F_H_ */
