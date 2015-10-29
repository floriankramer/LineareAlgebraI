/*
 * EntityBox.h
 *
 *  Created on: Aug 31, 2015
 *      Author: dwarf
 */

#ifndef ENTITYBOX_H_
#define ENTITYBOX_H_

#include "Entity.h"

namespace game {

class EntityBox: public Entity{
public:
	EntityBox();
	EntityBox(int width, int height);
	virtual ~EntityBox();
	virtual void render(float updateFactor);
};

} /* namespace game */

#endif /* ENTITYBOX_H_ */
